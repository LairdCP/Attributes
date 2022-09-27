#
# @file attribute_generator.py
#
# @brief Generate C code from JSON API document.
#
# Copyright (c) 2018-2022 Laird Connectivity
#
# SPDX-License-Identifier: Apache-2.0
#
import yaml
import json
import collections
import os
import inflection
import shutil
import sys
import math
import dollar_ref
from pathlib import Path
import string
from dataclasses import dataclass
import csv

JSON_INDENT = '  '

# Left Justified Field Widths
ID_WIDTH = 54
NAME_MACRO_WIDTH = 46
AV_FIELD_WIDTH = 20
AP_WIDTH = 36
DEFINE_WIDTH = 20
GS_CASE_WIDTH = 18
TYPE_WIDTH = 24
MIN_MAX_WIDTH = 20
OPTION_BITMASK_WIDTH = 6

DESTINATION_FOLDER = "components/attributes"
TABLE_FILE_NAME = "attr_table"

WRITABLE_FLAG = 0x1
READABLE_FLAG = 0x2
BROADCAST_FLAG = 0x8
SAVABLE_FLAG = 0x10
DEPRECATED_FLAG = 0x20
DISPLAY_OBSCURE_IN_SHOW_FLAG = 0x40
DISPLAY_HIDE_IN_SHOW_FLAG = 0x80
NOTIFY_IF_VALUE_UNCHANGED = 0x2000

# 64-bit number requires 20 characters (+ sign + terminator)
# This value is also larger than the maximum number of digits used for floats
MAX_NUMBER_STR_SIZE = 22
# 1/7 printed with default settings and %e: -1.428571e-01
MAX_FLOAT_STR_SIZE = 14

EMPTY_VALUE_STRING = '""'


@dataclass
class Skeleton:
    lst: list
    file_name: Path
    dest_api_folder: Path


def ToInt(b) -> str:
    return math.trunc(b)


def GetNumberField(d: dict, item: str):
    """ Handles items not being present (lazy get) """
    try:
        r = d[item]
        return r
    except:
        return 0.0


def GetBoolField(d: dict, item: str):
    try:
        r = d[item]
        return r
    except:
        return False


def GetStringField(d: dict, item: str):
    try:
        r = d[item]
        return r
    except:
        return ""


def GetDictionaryField(d: dict, item):
    r = {}
    try:
        r = d[item]
        return r
    except:
        return r


def PrintDuplicate(lst):
    for item, count in collections.Counter(lst).items():
        if count > 1:
            print(item)


def GenEnums(lst, enums, names, errno):
    for i, name, include_errno in zip(enums, names, errno):
        if len(i) != 0:
            snake = inflection.underscore(name)
            s = f"enum {snake}" + " {" + "\n"
            if include_errno:
                s += "\t" + snake.upper() + "_" + \
                    f"INCLUDE_ERRNO = INT32_MIN,\n"
            for key, value in i.items():
                s += "\t" + snake.upper() + "_" \
                    + key + " = " + str(value).lower() + ",\n"
            s += "};\n\n"
            lst.append(s)


def CountStr(N=12, chars=string.digits):
    """
    Return a string that counts the maximum number of characters of the
    string representation of value
    """
    return ''.join(string.digits[x % 10]for x in range(1, N + 1))


def UpdateVersion(yml_file: Path, parameters: list) -> None:
    """
    Read version (app.yaml file) of the form x.y.z
    Update version in parameter list (and api file) using top level document
    """
    version = "0.0.0"
    try:
        with open(yml_file, 'r') as f:
            data = yaml.load(f, Loader=yaml.FullLoader)
        version = data['info']['version']
        print(yml_file, f"{version=}")
    except:
        raise Exception("Unable to read API version")

    for value in parameters:
        if value['name'] == "api_version":
            value['x-default'] = version
            return

    raise Exception("Unable to find api_version attribute")


def Reserved0Check(parameters: list) -> None:
    """
    Location 0 is reserved because it makes valid ID test easier.
    """
    for value in parameters:
        if value['name'] == "reserved0":
            if value['x-id'] == 0:
                return

    raise Exception("Unable to find reserved0 with x-id of 0")


def CreateCompleteApiFile(api_src: Path, api_dest: Path, proj_name: str):
    """Aggregate individual files into one

    Args:
        api_src (Path): top level yml file
        attr_combine_path (Path): destination
        proj_name (str): project or board name (e.g., mg100, bt610)
    """
    ref = dollar_ref.resolve_file(str(api_src), '')

    # Remove the other component reference links
    content_descriptors = ref['components']['contentDescriptors']
    remove_device = []
    for value in content_descriptors:
        if "device_params" not in value:
            remove_device.append(value)
    for r in remove_device:
        del ref['components']['contentDescriptors'][r]

    # Add methods
    methods_used = []
    for method in ref['methods']:
        methods_used.extend(dollar_ref.pluck(method, 'methods'))
    ref.update({'methods': methods_used})

    attributes_used = []

    # Add attributes used in all versions
    attributes_list = ref['components']['contentDescriptors']['device_params']['x-device-parameters']
    for attr in attributes_list:
        attributes_used.extend(dollar_ref.pluck(attr, 'attributes'))

    device_params = ref['components']['contentDescriptors']['device_params']

    # Add board/project specific attributes
    if len(proj_name) > 0:
        try:
            proj = device_params[f'x-{proj_name}']
            for attr in proj:
                attributes_used.extend(dollar_ref.pluck(attr, 'attributes'))
        except:
            print(f"No {proj_name} specific attributes found")

    # Remove the array items from device_params that will not be used in the output file
    remove_items = []
    for value in device_params:
        if "name" not in value and "schema" not in value and "x-device-parameters" not in value:
            remove_items.append(value)
    for r in remove_items:
        del ref['components']['contentDescriptors']['device_params'][r]

    # Update with the modified keys
    device_params.update({'x-device-parameters': attributes_used})

    # Enumerate IDs
    # (IDs are not constant and are the deprecated method of referencing attributes)
    for k, value in enumerate(device_params['x-device-parameters']):
        value['x-id'] = k

    UpdateVersion(api_src, device_params['x-device-parameters'])

    Reserved0Check(device_params['x-device-parameters'])

    # Generate the API files in both yaml and json
    with open(api_dest.joinpath("api.yml"), 'w') as f:
        yaml.dump(ref, f, indent=2, sort_keys=False)
    with open(api_dest.joinpath("api.json"), 'w') as f:
        json.dump(ref, f, indent=JSON_INDENT, sort_keys=False)


class attributes:
    def __init__(self, api_src: Path):

        # The following items are loaded from the configuration file
        self.parameter_list = 0
        self.projectAttributeCount = 0
        self.maxNameLength = 0
        self.largestNumberSize = 0

        self.id = []
        self.max = []
        self.min = []
        self.name = []
        self.type = []
        self.ctype = []
        self.lockable = []
        self.broadcast = []
        self.stringMax = []
        self.arraySize = []
        self.default = []
        self.writable = []
        self.readable = []
        self.savable = []
        self.deprecated = []
        self.displayOptions = []
        self.validator = []
        self.prepare = []
        self.enum = []
        self.enum_include_errno = []
        self.methodEnums = []
        self.methodEnumNames = []
        self.methodEnumIncludeErrno = []
        self.enableFpuCheck = 0

        self.LoadConfig(api_src)

    def LoadConfig(self, api_src: Path) -> None:
        with open(api_src, 'r') as f:
            data = yaml.load(f, Loader=yaml.FullLoader)
            self.parameter_list = data['components']['contentDescriptors']['device_params']['x-device-parameters']

            # Extract the properties for each parameter
            for p in self.parameter_list:
                self.name.append(p['name'])
                self.id.append(p['x-id'])
                self.default.append(p['x-default'])
                self.lockable.append(GetBoolField(p, 'x-lockable'))
                self.broadcast.append(GetBoolField(p, 'x-broadcast'))
                self.readable.append(GetBoolField(p, 'x-readable'))
                self.writable.append(GetBoolField(p, 'x-writable'))
                self.savable.append(GetBoolField(p, 'x-savable'))
                self.deprecated.append(GetBoolField(p, 'x-deprecated'))
                # Required schema fields
                a = p['schema']
                self.CheckValidType(a['type'], p['x-ctype'], p['name'])
                self.CheckFloat(p['x-ctype'])
                self.type.append(a['type'])
                self.ctype.append(p['x-ctype'])

                # Min and max are lengths
                if a['type'] == "string":
                    # stringMax string size is only required for strings.
                    self.stringMax.append(GetNumberField(a, 'maxLength'))
                    self.max.append(GetNumberField(a, 'maxLength'))
                    self.min.append(GetNumberField(a, 'minLength'))
                elif a['type'] == "array":
                    self.max.append(GetNumberField(a, 'maxItems'))
                    self.min.append(GetNumberField(a, 'minItems'))
                else:
                    self.max.append(GetNumberField(a, 'maximum'))
                    self.min.append(GetNumberField(a, 'minimum'))

                # Optional schema
                self.enum.append(GetDictionaryField(a, 'enum'))

                # Optional fields have a default value
                self.arraySize.append(GetNumberField(p, 'x-array-size'))
                self.validator.append(GetStringField(p, 'x-validator'))
                self.prepare.append(GetBoolField(p, 'x-prepare'))

                self.ProcessDisplayOptions(p)

                self.enum_include_errno.append(
                    GetBoolField(p, 'x-enum-include-errno'))

            self.projectAttributeCount = len(self.name)
            print(f"Total Attributes = {self.projectAttributeCount}")
            pass

    def ProcessDisplayOptions(self, p: any):
        obscureInShow = GetBoolField(p, 'x-obscure-in-show')
        hideInShow = GetBoolField(p, 'x-hide-in-show')
        showUnlockedOverride = GetBoolField(p, 'x-show-unlocked-override')
        obscureInDump = GetBoolField(p, 'x-obscure-in-dump')
        hideInDump = GetBoolField(p, 'x-hide-in-dump')
        dumpUnlockedOverride = GetBoolField(p, 'x-dump-unlocked-override')
        showOnChange = GetBoolField(p, 'x-show-on-change')
        notifyIfUnchanged = GetBoolField(p, 'x-notify-if-unchanged')
        displayOptionsBitmask = 0
        name = p['name']

        if (hideInShow == 1 and obscureInShow == 1):
            raise Exception("Cannot both hide and obscure attribute " +
                            name + " (in show)")

        if (obscureInShow == 1):
            displayOptionsBitmask |= DISPLAY_OBSCURE_IN_SHOW_FLAG

        if (hideInShow == 1):
            displayOptionsBitmask |= DISPLAY_HIDE_IN_SHOW_FLAG

        if (showUnlockedOverride == 1):
            print("deprecated - show unlocked override - " + name)

        if (obscureInDump == 1):
            print("deprecated - obscure in dump - " + name)

        if (hideInDump == 1):
            print("deprecated - hide in dump - " + name)

        if (dumpUnlockedOverride == 1):
            print("deprecated - dump unlocked override - " + name)

        if (showOnChange == 1):
            print("deprecated - show on change - " + name)

        if (notifyIfUnchanged == 1):
            displayOptionsBitmask |= NOTIFY_IF_VALUE_UNCHANGED

        self.displayOptions.append(displayOptionsBitmask)

    def GetType(self, index: int) -> str:
        kind = self.ctype[index]
        array_size = self.arraySize[index]
        numberSize = 0
        s = "ATTR_TYPE_"
        if kind == "string":
            s += "STRING"
        elif array_size != 0:
            # only u8 array is supported at this time
            s += "BYTE_ARRAY"
        elif kind == "float":
            s += "FLOAT"
            numberSize = 4
        elif kind == "int8_t":
            s += "S8"
            numberSize = 1
        elif kind == "int16_t":
            s += "S16"
            numberSize = 2
        elif kind == "int32_t":
            s += "S32"
            numberSize = 4
        elif kind == "int64_t":
            s += "S64"
            numberSize = 8
        elif kind == "bool":
            s += "BOOL"
            numberSize = 1
        elif kind == "uint8_t":
            s += "U8"
            numberSize = 1
        elif kind == "uint16_t":
            s += "U16"
            numberSize = 2
        elif kind == "uint32_t":
            s += "U32"
            numberSize = 4
        elif kind == "uint64_t":
            s += "U64"
            numberSize = 8
        elif kind == "atomic_t":
            s += "ATOMIC"
            numberSize = 4
        else:
            s += "UNKNOWN"
            raise TypeError(f"Type {kind} not expected")

        if (numberSize > self.largestNumberSize):
            self.largestNumberSize = numberSize

        return s

    def GetMaxChars(self, index: int) -> int:
        """Maximum number of characters to represent value as a string with sign
        """
        kind = self.ctype[index]
        array_size = self.arraySize[index]
        size = 0
        if kind == "string":
            size = self.max[index]
        elif array_size != 0:
            size = array_size * 2
        elif kind == "float":
            size = MAX_FLOAT_STR_SIZE
        elif kind == "int8_t":
            size = len("-128")
        elif kind == "int16_t":
            size = len("-32768")
        elif kind == "int32_t":
            size = len("-2147483648")
        elif kind == "int64_t":
            size = len("-9223372036854775808")
        elif kind == "bool":
            size = 1
        elif kind == "uint8_t":
            size = len("255")
        elif kind == "uint16_t":
            size = len("65535")
        elif kind == "uint32_t":
            size = len("4294967295")
        elif kind == "uint64_t":
            size = len("18446744073709551615")
        elif kind == "atomic_t":
            size = len("4294967295")
        else:
            raise TypeError(f"Type {kind} not expected")

        return size

    def GetAttributeMacro(self, index: int) -> str:
        """Get the c-macro for the RW or RO attribute (array vs non-array pointer)"""
        kind = self.ctype[index]
        savable = self.savable[index]
        name = self.name[index]
        enum = len(self.enum[index]) != 0
        if kind == "string":
            if savable:
                s = "RW_ATTRS("
            else:
                s = "RO_ATTRS("
        else:
            if savable:
                if enum:
                    s = "RW_ATTRE("
                else:
                    s = "RW_ATTRX("
            else:
                if enum:
                    s = "RO_ATTRE("
                else:
                    s = "RO_ATTRX("

        s += name + ")"
        return s.ljust(NAME_MACRO_WIDTH)

    def GetValidatorString(self, index: int) -> str:
        """Use custom validator if it exists.  Otherwise use validator based on type"""
        kind = self.ctype[index]
        array_size = self.arraySize[index]
        validator = self.validator[index]
        if validator != "":
            s = "av_" + validator
        elif kind == "string":
            s = "av_" + "string"
        elif array_size != 0:
            s = "av_" + "array"
        else:
            s = "av_" + kind.replace('_t', '')

        return s.ljust(AV_FIELD_WIDTH)

    def CreateMinMaxString(self, index: int) -> str:
        """
        Create the min/max portion of the attribute table entry
        For strings these values are the min/max lengths.
        """
        kind = self.ctype[index]
        i_min = self.min[index]
        i_max = self.max[index]
        if kind == "float":
            s_min = ".min.fx = " + str(i_min)
            s_max = ".max.fx = " + str(i_max)
            if (("." or "e") not in str(i_min)):
                s_min = s_min + ".0"
            if (("." or "e") not in str(i_max)):
                s_max = s_max + ".0"
        else:
            i_min = int(i_min)
            i_max = int(i_max)
            # This will check if it starts with an "i" to indicate it is a signed number
            if (kind[0] == "i"):
                s_min = f".min.sx = " + str(i_min)
                s_max = f".max.sx = " + str(i_max)
            else:
                s_min = f".min.ux = " + str(i_min)
                s_max = f".max.ux = " + str(i_max)

        return s_min.ljust(MIN_MAX_WIDTH) + ", " + s_max.ljust(MIN_MAX_WIDTH)

    def GetIndex(self, name: str) -> int:
        """ Get the index for a key/name """
        for i in range(self.projectAttributeCount):
            if self.name[i] == name:
                return i
        return -1

    def CreateAttrTable(self) -> str:
        """
        Create the attribute (property) table from the dictionary of lists
        """
        attributeTable = []
        attributeTable.append(
            "const struct attr_table_entry ATTR_TABLE[ATTR_TABLE_SIZE] = {\n")
        for i in range(self.projectAttributeCount):
            opts = ToInt(self.displayOptions[i])
            if (self.writable[i]):
                opts |= WRITABLE_FLAG
            if (self.readable[i]):
                opts |= READABLE_FLAG
            if (self.lockable[i]):
                print(f"deprecated - lockable - {self.name[i]}")
            if (self.broadcast[i]):
                opts |= BROADCAST_FLAG
            if (self.savable[i]):
                opts |= SAVABLE_FLAG
            if (self.deprecated[i]):
                opts |= DEPRECATED_FLAG
            opts = hex(opts)

            result = f"\t[{i:<3}] = " + "{ " \
                + f"{self.GetAttributeMacro(i)}, {self.GetType(i).ljust(TYPE_WIDTH)}, " \
                + f"{opts.ljust(OPTION_BITMASK_WIDTH)}, " \
                + f"{self.GetValidatorString(i)}, {self.GetPrepareString(i)}, " \
                + f"{self.CreateMinMaxString(i)}"
            if (i == (self.projectAttributeCount - 1)):
                result = result + "}\n"
            else:
                result = result + "},\n"
            attributeTable.append(result)
        attributeTable.append("};\n")

        string = ''.join(attributeTable)
        return string

    def GenerateVariable(self, kind: str, maximum: int) -> str:
        """ Determine if variable is an string/array """
        if kind == "char":
            # Add one for the NULL character
            return f"[{maximum} + 1]"
        elif maximum != 0:
            # This is an array (byte, float, ...)
            return f"[{maximum}]"
        else:
            return ""

    def GetDefault(self, kind: str, default: str) -> str:
        if default == "NA":
            if kind == "char":
                return '""'
            elif kind == "float":
                return 0.0
            else:
                return 0
        else:
            if kind == "char":
                return ('"' + default + '"')
            else:
                return default

    def GetPrepareString(self, index: int) -> str:
        name = self.name[index]
        if self.prepare[index]:
            s = "attr_prepare_" + name
        else:
            s = "NULL"

        return s.ljust(AP_WIDTH)

    def CheckValidType(self, type: str, ctype: str, name: str):
        """
        This will check the type and c-type field in the schema and will stop the generator if they don't match.
        """
        # Check type
        if type == "null":
            pass
        elif type == "boolean":
            pass
        elif type == "object":
            pass
        elif type == "array":
            pass
        elif type == "number":
            pass
        elif type == "string":
            pass
        elif type == "integer":
            pass
        else:
            raise TypeError(
                f"Type {type} not expected in type in the schema for {name} parameter")

        # Check c-type
        if ctype == "string":
            pass
        elif ctype == "float":
            pass
        elif ctype == "int8_t":
            pass
        elif ctype == "int16_t":
            pass
        elif ctype == "int32_t":
            pass
        elif ctype == "int64_t":
            pass
        elif ctype == "bool":
            pass
        elif ctype == "uint8_t":
            pass
        elif ctype == "uint16_t":
            pass
        elif ctype == "uint32_t":
            pass
        elif ctype == "uint64_t":
            pass
        elif ctype == "atomic_t":
            pass
        else:
            raise TypeError(
                f"Type {ctype} not expected in c-type for {name} parameter")

    def CheckFloat(self, ctype: str):
        """
        Check for floating type so that compile time FPU check can be added.
        """
        if ctype == "float":
            self.enableFpuCheck = 1

    def CheckForDuplicates(self) -> None:
        """
        Check for duplicate parameter IDs or names.
        """
        if len(set(self.id)) != len(self.id):
            # This indicates a problem in the generation script
            PrintDuplicate(self.name)
            raise Exception("Duplicate attribute ID in project")

        if len(set(self.name)) != len(self.name):
            # This indicates duplicates in the yml input files
            PrintDuplicate(self.name)
            raise Exception("Duplicate attribute name in project")

    def UpdateFiles(self, dest_api_folder: Path, dest_src_folder: Path, dest_hdr_folder: Path) -> None:
        """
        Update the attribute c/h files.
        """

        dest_src_file = dest_src_folder.joinpath("attr_table.c")
        dest_hdr_file = dest_hdr_folder.joinpath("attr_table.h")
        dest_util_file = dest_hdr_folder.joinpath("attr_util.h")

        self.CheckForDuplicates()
        self.CreateSourceFile(self.CreateSkeleton(
            dest_src_file, dest_api_folder))
        self.CreateHeaderFile(self.CreateSkeleton(
            dest_hdr_file, dest_api_folder))
        self.CreateUtilFile(self.CreateSkeleton(
            dest_util_file, dest_api_folder))

        self.MakeDefaultFile(dest_api_folder)
        self.MakeWritableFile(dest_api_folder)

    def CreateInsertionList(self, name: str) -> list:
        """
        Read in the c/h file and create a list of strings that
        is ready for the attribute information to be inserted
        """
        print("Reading " + name)
        lst = []
        new_line_skip_flag = False
        with open(name, 'r') as fin:
            copying = True
            for line in fin:
                if "pystart" in line:
                    copying = False
                elif "pyend" in line:
                    copying = True
                elif (("\n" == line) and new_line_skip_flag):
                    # Don't copy blank new line, skip over this
                    pass
                elif copying:
                    lst.append(line)
                    if "Type Definitions" in line:
                        new_line_skip_flag = True

        return lst

    def CreateStruct(self, category: str, default_values: bool) -> str:
        """
        Creates the structures and default values for RW and RO attributes.
        Writable but non-savable values are populated in the RO structure.
        """
        struct = []
        if default_values:
            struct.append(f"static const {category}_attribute_t")
            struct.append(f" DEFAULT_{category.upper()}_ATTRIBUTE_VALUES")
            struct.append(" =  {\n")
        else:
            struct.append(f"typedef struct {category}_attribute ")
            struct.append("{\n")

        for i in range(self.projectAttributeCount):
            savable = self.savable[i]
            if (category == 'rw' and savable) or (
                    (category == 'ro') and not savable):
                name = self.name[i]
                # string is required in test tool, C requires char type
                if self.ctype[i] == "string":
                    kind = "char"
                    i_max = self.max[i]
                elif len(self.enum[i]) != 0:
                    snake = inflection.underscore(self.name[i])
                    kind = f"enum {snake}"
                    i_max = self.arraySize[i]
                else:
                    kind = self.ctype[i]
                    i_max = self.arraySize[i]

                default = self.default[i]

                # Use tabs because we use tabs with Zephyr/clang-format.
                if default_values:
                    result = f"\t.{name} = {self.GetDefault(kind, default)}"
                    if(i == (self.projectAttributeCount - 1)):
                        result = result + "\n"
                    else:
                        result = result + ",\n"
                else:
                    result = f"\t{kind} {name}{self.GenerateVariable(kind, i_max)};" + "\n"

                struct.append(result)

        struct.append("}")
        if default_values:
            struct.append(";\n")
        else:
            struct.append(f" {category}_attribute_t;\n")

        return ''.join(struct)

    def CreateSourceFile(self, x: Skeleton) -> None:
        """Create the settings/attributes/properties *.c file"""
        print(f"Writing {x.file_name}")
        with open(x.file_name, 'w') as fout:
            for index, line in enumerate(x.lst):
                next_line = index + 1
                if "pystart - " in line:
                    if "table" in line:
                        x.lst.insert(next_line, self.CreateAttrTable())
                    elif "rw attributes" in line:
                        x.lst.insert(next_line, self.CreateStruct("rw", False))
                    elif "rw defaults" in line:
                        x.lst.insert(next_line, self.CreateStruct("rw", True))
                    elif "ro attributes" in line:
                        x.lst.insert(next_line, self.CreateStruct("ro", False))
                    elif "ro defaults" in line:
                        x.lst.insert(next_line, self.CreateStruct("ro", True))
                    elif "get string" in line:
                        x.lst.insert(
                            next_line, self.CreateGetStringFunctions())

            fout.writelines(x.lst)

    def CreateIds(self) -> str:
        """Create attribute ids for header file"""
        ids = []
        number_ids = len(self.name)
        for id in range(number_ids):
            result = f"#define ATTR_ID_{self.name[id]}".ljust(
                ID_WIDTH) + str(id) + "\n"
            ids.append(result)
        return ''.join(ids)

    def CreateIsEnabledDefines(self) -> str:
        """Create defines that allow IS_ENABLED macros to work"""
        ids = []
        for id in self.id:
            result = f"#define _ATTRX{id} _YYYY,\n"
            ids.append(result)
        return ''.join(ids)

    def CreateConstants(self, dest_api_folder: Path) -> str:
        """Create some definitions for header file"""
        defs = []
        maxBinSize = ToInt(max(self.arraySize))

        if (self.largestNumberSize > maxBinSize):
            maxBinSize = self.largestNumberSize

        self.maxNameLength = len(max(self.name, key=len))

        defs.append(self.JustifyDefine(
            "TABLE_SIZE", "", max(self.id)+1))
        defs.append(self.JustifyDefine(
            "TABLE_MAX_ID", "", max(self.id)))
        defs.append(self.JustifyDefine(
            "TABLE_WRITABLE_COUNT", "", sum(self.writable)))
        defs.append(self.JustifyDefine("MAX_STR_LENGTH", "",
                                       max(self.stringMax)))
        defs.append(self.JustifyDefine("MAX_STR_SIZE", "",
                                       max(self.stringMax) + 1))
        defs.append(self.JustifyDefine("MAX_BIN_SIZE", "",
                                       maxBinSize))
        defs.append(self.JustifyDefine("MAX_INT_SIZE", "",
                                       self.largestNumberSize))
        defs.append(self.JustifyDefine("MAX_KEY_NAME_SIZE", "",
                                       self.maxNameLength + 1))
        defs.append(self.JustifyDefine("MAX_VALUE_SIZE", "",
                                       max(max(self.stringMax), (2 * maxBinSize), MAX_NUMBER_STR_SIZE) + 1))
        defs.append(self.JustifyDefine("MAX_FILE_SIZE", "",
                                       self.EstimateMaxFileSize(dest_api_folder)))
        defs.append(self.JustifyDefine("ENABLE_FPU_CHECK", "",
                                       self.enableFpuCheck))

        self.CreateMaxStringSizes(defs)
        self.CreateArraySizes(defs)
        return ''.join(defs)

    def JustifyDefine(self, key: str, suffix: str, value: int) -> str:
        width = self.maxNameLength + DEFINE_WIDTH
        if len(suffix) != 0:
            name = key + "_" + suffix
        else:
            name = key
        return "#define ATTR_" + name.ljust(width) + f" {str(value)}\n"

    def CreateMaxStringSizes(self, lst: list) -> str:
        """
        Create max string size definitions (size includes NULL terminator)
        The pystart and pyend are done in the parent function
        """
        lst.append("\n")
        lst.append("/* Attribute Max String Lengths */\n")
        for kind, length, name in zip(self.ctype, self.max, self.name):
            if kind == "string":
                snake = inflection.underscore(name).upper()
                lst.append(self.JustifyDefine(
                    snake, "MAX_STR_SIZE", length + 1))

        return ''.join(lst)

    def EstimateMaxFileSize(self, dest_api_folder: Path) -> str:
        """
        Return max size of file (without comments)
        Print file with name=<count of max size in chars>\n
        """
        size = 0
        delimiter_chars = 2
        try:
            with open(dest_api_folder.joinpath("max.txt"), 'w') as f:
                f.write("# Maximum File Size (without comments) for Attributes \n")
                for i in range(self.projectAttributeCount):
                    chars = self.GetMaxChars(i)
                    size += delimiter_chars + len(self.name[i]) + chars
                    f.write(f"{self.name[i]}={CountStr(chars)}\n")
        except:
            print("Unable to open max length file for writing")

        return size

    def MakeDefaultFile(self, dest_api_folder: Path):
        try:
            with open(dest_api_folder.joinpath("defaults.txt"), 'w') as f:
                f.write("# Default Values for Attributes \n")
                for i in range(self.projectAttributeCount):
                    default = str(self.default[i])
                    if len(default) == 0:
                        default = EMPTY_VALUE_STRING
                    f.write(f"{self.name[i]}={default}\n")
        except:
            print("Error writing to default attributes file")

    def MakeWritableFile(self, dest_api_folder: Path):
        try:
            with open(dest_api_folder.joinpath("writable.txt"), 'w') as f:
                f.write("# Writable Attributes\n")
                for i in range(self.projectAttributeCount):
                    if self.writable[i]:
                        default = str(self.default[i])
                        if len(default) == 0:
                            default = EMPTY_VALUE_STRING
                        f.write(f"{self.name[i]}={default}\n")
        except:
            print("Error writing to default attributes file")

    def CreateArraySizes(self, lst: list) -> str:
        """
        Create array size definitions (only byte arrays are currently supported)
        The pystart and pyend are done in the parent function
        """
        array_found = False
        lst.append("\n")
        lst.append("/* Attribute Byte Array Lengths */\n")
        for size, name in zip(self.arraySize, self.name):
            if size != 0:
                name = inflection.underscore(name).upper()
                lst.append(self.JustifyDefine(name, "SIZE", size))
                array_found = True
        if (not array_found):
            # Remove the comment about array lengths since no arrays were found
            lst.pop()
            lst.pop()
        return ''.join(lst)

    def GeneratePrepareFunctionName(self, index: int) -> str:
        return

    def CreatePrepare(self) -> str:
        """Create prototypes for prepared read functions"""
        lst = []
        for create, name in zip(self.prepare, self.name):
            if create:
                s = "int attr_prepare_" + name + "(void);\n"

                lst.append(s)
        return ''.join(lst)

    def CreateEnums(self) -> str:
        lst = []
        GenEnums(lst, self.enum, self.name, self.enum_include_errno)
        # Generator for commands/methods that have enums
        GenEnums(lst, self.methodEnums, self.methodEnumNames,
                 self.methodEnumIncludeErrno)
        return ''.join(lst)

    def CreateEnumSizeCheck(self) -> str:
        """
        The C-compiler generates the size of the enum based on the number
        elements.
        """
        lst = []
        for i, name in zip(self.enum, self.name):
            if len(i) != 0:
                snake = inflection.underscore(name)
                t = self.GetType(self.name.index(name))
                s = f"BUILD_ASSERT(sizeof(enum {snake}) == "
                s += f"{t.replace('TYPE','SIZE')});\n"
                lst.append(s)
        return ''.join(lst)

    def CreateGetStringPrototypes(self) -> str:
        """
        Get string function uses integer type so that signature is generic.
        """
        lst = []
        for enum, name in zip(self.enum, self.name):
            if len(enum) != 0:
                snake = inflection.underscore(name)
                s = f"const char *const attr_get_string_{snake}(int value);\n"
                lst.append(s)
        return ''.join(lst)

    def CreateGetStringFunctions(self) -> str:
        """Generate functions that return a string of each enum"""
        lst = []
        for enum, camel, include_errno in zip(self.enum, self.name, self.enum_include_errno):
            if len(enum) != 0:
                snake = inflection.underscore(camel)
                s = f"const char *const attr_get_string_{snake}(int value)\n"
                s += "{\n"
                s += "\tswitch (value) {\n"
                for key, value in enum.items():
                    s += '\t\t' + f'case {str(value).lower()}:'.ljust(GS_CASE_WIDTH) + \
                        'return "' + inflection.titleize(key) + '";\n'
                s += '\t\t' + 'default:'.ljust(GS_CASE_WIDTH)

                if include_errno:
                    s += 'return errno_str_get(value);\n'
                else:
                    s += 'return "?";\n'

                s += "\t}\n}\n\n"

                lst.append(s)

        string = ''.join(lst)

        # Handle special cases
        with open(Path(os.path.dirname(__file__)).joinpath('enum_replace.csv')) as csv_file:
            reader = csv.reader(csv_file, delimiter=',')
            for old, new in reader:
                old = old.replace('"', '').replace(' ', '')
                new = new.replace('"', '').replace(' ', '')
                string = string.replace(old, new)

        return string

    def CreateHeaderFile(self, x: Skeleton) -> None:
        """Create the attribute header file"""
        print(f"Writing {x.file_name}")
        with open(x.file_name, 'w') as fout:
            for index, line in enumerate(x.lst):
                next_line = index + 1
                if "pystart - " in line:
                    if "attribute ids" in line:
                        x.lst.insert(next_line, self.CreateIds())
                    elif "attribute indices" in line:
                        x.lst.insert(next_line, self.CreateIndices())
                    elif "attribute constants" in line:
                        x.lst.insert(
                            next_line, self.CreateConstants(x.dest_api_folder))
                    elif "prepare for read" in line:
                        x.lst.insert(
                            next_line, self.CreatePrepare())
                    elif "enumerations" in line:
                        x.lst.insert(
                            next_line, self.CreateEnums())
                    elif "enum size check" in line:
                        x.lst.insert(
                            next_line, self.CreateEnumSizeCheck())
                    elif "get string" in line:
                        x.lst.insert(
                            next_line, self.CreateGetStringPrototypes())

            fout.writelines(x.lst)

    def CreateSkeleton(self, file_name: Path, dest_api_folder: Path) -> Skeleton:
        """
        Read in the c/h file and create a list (of strings) that
        doesn't include the regions that are going to be generated/inserted later.
        """
        x = Skeleton([], file_name, dest_api_folder)
        print(f"Reading {x.file_name}")
        with open(x.file_name, 'r') as fin:
            copying = True
            for line in fin:
                if "pystart" in line:
                    x.lst.append(line)
                    copying = False
                elif "pyend" in line:
                    x.lst.append(line)
                    copying = True
                elif copying:
                    x.lst.append(line)

        return x

    def CreateUtilFile(self, x: Skeleton) -> None:
        """Create the attribute header utility file"""
        print(f"Writing {x.file_name}")
        with open(x.file_name, 'w') as fout:
            for index, line in enumerate(x.lst):
                next_line = index + 1
                if "pystart - " in line:
                    if "attribute present" in line:
                        x.lst.insert(next_line, self.CreateIsEnabledDefines())

            fout.writelines(x.lst)


def generate(api_src: Path, dest_dir: Path, proj_key: str):
    """ Generate API and attribute files for a specific project """
    dest_api_folder = dest_dir.joinpath(proj_key, "attributes")  # ?
    dest_inc_folder = dest_dir.joinpath(proj_key, "include")
    dest_src_folder = dest_dir.joinpath(proj_key, "src")

    # Generate folders
    try:
        if (not Path.exists(dest_api_folder)):
            Path.mkdir(dest_api_folder, parents=True)
        if (not Path.exists(dest_inc_folder)):
            Path.mkdir(dest_inc_folder)
        if (not Path.exists(dest_src_folder)):
            Path.mkdir(dest_src_folder)
    except:
        raise Exception("Unable to create output folder")

    # Copy templates from script dir to destination
    template_dir = Path(os.path.dirname(__file__)).joinpath("template")
    try:
        files = ["attr_table.c"]
        for x in files:
            shutil.copy(template_dir.joinpath(x), dest_src_folder.joinpath(x))
        files = ["attr_table.h", "attr_util.h"]
        for x in files:
            shutil.copy(template_dir.joinpath(x), dest_inc_folder.joinpath(x))
    except:
        raise Exception("Unable to copy templates")

    CreateCompleteApiFile(api_src, dest_api_folder, proj_key)

    a = attributes(dest_api_folder.joinpath("api.yml"))
    a.UpdateFiles(dest_api_folder, dest_src_folder, dest_inc_folder)


if __name__ == "__main__":
    # argv[0] can be the script name
    if len(sys.argv) < 2:
        raise ValueError(
            'No api_src path entered for the yaml file listing the attribute parameters')

    try:
        # Location of top level api document is used to make destination dir
        api_src = Path(sys.argv[1])
        dest_dir = api_src.joinpath(DESTINATION_FOLDER)
        api_src = Path(api_src).joinpath('app.yml')
    except:
        raise ValueError('Unable to generate paths')

    # Ensure first parameter + name is valid
    if not Path.exists(api_src):
        raise ValueError(f'API source file invalid "{str(api_src)}"')

    # Make list of key names (blank name when list isn't present)
    key_names = []
    for i, arg in enumerate(sys.argv):
        if i > 1:
            key_names.append(arg)
    if len(key_names) == 0:
        key_names.append("")

    for key in key_names:
        generate(api_src, dest_dir, key)
