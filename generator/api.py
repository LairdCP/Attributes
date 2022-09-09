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

template_file_path = "%COMPONENTS%/app_source_file_template/templates/"
attr_header_file_path = "%INDEX%/components/attributes/%KEY_NAME%/include/"
attr_source_file_path = "%INDEX%/components/attributes/%KEY_NAME%/src/"
attr_combine_file_path = "%INDEX%/components/attributes/%KEY_NAME%/attributes/"
TABLE_FILE_NAME = "attr_table"
DEFAULT_FILE_NAME = "api"

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


def UpdateVersion(yml_file: str, parameters: list) -> None:
    """
    Read version (app.yaml file) of the form x.y.z
    Update version in parameter list (and api file) using top level document
    """
    version = "0.0.0"
    try:
        with open(yml_file, 'r') as f:
            data = yaml.load(f, Loader=yaml.FullLoader)
        version = data['info']['version']
        print(yml_file, f"Version = {version}")
    except:
        raise Exception("Unable to read API version")

    for value in parameters:
        if value['name'] == "api_version":
            value['x-default'] = version
            return

    raise Exception("Unable to find api_version attribute")


def CreateCompleteAttributeFile(location: str, attr_combine_path: str, key_names: list):
    """Aggregate individual files into one

    Args:
        location (str): top level yml file
        attr_combine_path (str): destination
        key_names (list): project key names (e.g., mg100, bt610)
    """
    json_file_name = DEFAULT_FILE_NAME + ".json"
    yaml_file_name = DEFAULT_FILE_NAME + ".yml"
    ref = dollar_ref.resolve_file(location, '')

    try:
        os.makedirs(attr_combine_path)
        print("Directory '% s' created" % attr_combine_path)
    except:
        pass

    # Remove the other component reference links
    content_descriptors = ref['components']['contentDescriptors']
    remove_device = []
    for k, value in enumerate(content_descriptors):
        if "device_params" not in value:
            remove_device.append(value)
    for r in remove_device:
        del ref['components']['contentDescriptors'][r]

    # Organize the methods
    methods_used = []
    for method in ref['methods']:
        methods_used.extend(dollar_ref.pluck(method, 'methods'))
    ref.update({'methods': methods_used})

    # Organize the attributes
    attributes_used = []
    attributes_list = ref['components']['contentDescriptors']['device_params']['x-device-parameters']
    for attr in attributes_list:
        attributes_used.extend(dollar_ref.pluck(attr, 'attributes'))

    # Combine all the selected project parameters
    project_list = []
    device_params = ref['components']['contentDescriptors']['device_params']
    for key in key_names:
        if f"x-{key}" in device_params:
            project_list = ref['components']['contentDescriptors'][
                'device_params'][f'x-{key}']
    for project in project_list:
        attributes_used.extend(dollar_ref.pluck(project, 'attributes'))

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

    UpdateVersion(location, device_params['x-device-parameters'])

    # Generate the attribute files for both yaml and json
    with open(os.path.join(attr_combine_path, yaml_file_name), 'w') as f:
        yaml.dump(ref, f, indent=2, sort_keys=False)
    with open(os.path.join(attr_combine_path, json_file_name), 'w') as f:
        json.dump(ref, f, indent=JSON_INDENT, sort_keys=False)


class attributes:
    def __init__(self, fname: str):

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

        # Process the yaml version of the file
        self.LoadConfig(fname + ".yml")

    def LoadConfig(self, fname: str) -> None:
        with open(fname, 'r') as f:
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
        attributeTable.append("/* pystart - table */\n")
        attributeTable.append(
            "/* If min == max then range isn't checked. */\n\n")
        attributeTable.append(
            "/* index....name.....................type.flags.validator..prepare..min.max. */\n")
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
        attributeTable.append("/* pyend */\n\n")

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

    def UpdateFiles(self, header_file_path: str, source_file_path: str) -> None:
        """
        Update the attribute c/h files.
        """
        self.CheckForDuplicates()

        self.CreateSourceFile(source_file_path,
                              self.CreateInsertionList(source_file_path))
        self.CreateAttributeHeaderFile(header_file_path,
                                       self.CreateInsertionList(header_file_path))
        self.MakeDefaultFile()
        self.MakeWritableFile()

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

    def CreateLocalConstants(self) -> str:
        """
        Creates the defines needed within the source file
        """
        constant_names = []
        constant_names.append("/* pystart - RW/RO short defines */\n")
        constant_names.append("#define DRW DEFAULT_RW_ATTRIBUTE_VALUES\n")
        constant_names.append("#define DRO DEFAULT_RO_ATTRIBUTE_VALUES\n")
        constant_names.append("#define y true\n")
        constant_names.append("#define n false\n")
        constant_names.append("/* pyend */\n\n")
        return ''.join(constant_names)

    def CreateLocalDefines(self) -> str:
        """
        Creates the static variables needed within the source file
        """
        define_names = []
        define_names.append("/* pystart - static define */\n")
        define_names.append("static rw_attribute_t rw;\n")
        define_names.append("static ro_attribute_t ro;\n")
        define_names.append("/* pyend */\n\n")
        return ''.join(define_names)

    def CreateStruct(self, category: str, default_values: bool) -> str:
        """
        Creates the structures and default values for RW and RO attributes.
        Writable but non-savable values are populated in the RO structure.
        """
        struct = []
        if default_values:
            struct.append(f"/* pystart - {category} defaults */\n")
            struct.append(f"static const {category}_attribute_t")
            struct.append(f" DEFAULT_{category.upper()}_ATTRIBUTE_VALUES")
            struct.append(" =  {\n")
        else:
            struct.append(f"/* pystart - {category} attributes */\n")
            struct.append(f"typedef struct {category}_attribute ")
            struct.append("{\n")
        for i in range(self.projectAttributeCount):
            savable = self.savable[i]
            writable = self.writable[i]
            readable = self.readable[i]
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
        struct.append("/* pyend */\n")
        struct.append("\n")
        string = ''.join(struct)
        return string

    def CreateGlobalConstants(self) -> str:
        """
        Creates the global defines needed within the source file that have calls outside the file
        """
        global_names = []
        global_names.append("/* pystart - table */\n")
        global_names.append("/**\n * @brief Table shorthand\n")
        global_names.append(" *\n * @ref CreateStruct (Python script)\n")
        global_names.append(
            " * Writable but non-savable values are populated using RO macro.\n")
        global_names.append(
            " *\n *.........name...value...default....size...writable..readable..get enum str\n")
        global_names.append(" */\n")
        global_names.append(
            "#define RW_ATTRS(n) STRINGIFY(n), rw.n, DRW.n, sizeof(rw.n), NULL\n")
        global_names.append(
            "#define RW_ATTRX(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), NULL\n")
        global_names.append(
            "#define RW_ATTRE(n) STRINGIFY(n), &rw.n, &DRW.n, sizeof(rw.n), attr_get_string_ ## n\n")
        global_names.append(
            "#define RO_ATTRS(n) STRINGIFY(n), ro.n, DRO.n, sizeof(ro.n), NULL\n")
        global_names.append(
            "#define RO_ATTRX(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), NULL\n")
        global_names.append(
            "#define RO_ATTRE(n) STRINGIFY(n), &ro.n, &DRO.n, sizeof(ro.n), attr_get_string_ ## n\n")
        global_names.append("/* pyend */\n\n")
        return ''.join(global_names)

    def CreateSourceFile(self, source_file_path: str, lst: list) -> None:
        """Create the settings/attributes/properties *.c file"""
        print("Writing " + source_file_path)
        remove_list = []
        with open(source_file_path, 'w') as fout:
            for index, line in enumerate(lst):
                next_line = index + 1
                if "Includes" in line:
                    next_line = next_line + 1
                    lst.insert(next_line,  self.AddIncludes(True))
                elif "Local Constant" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateLocalConstants())
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreateStruct("rw", False))
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateStruct("rw", True))
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreateStruct("ro", False))
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateStruct("ro", True))
                elif "Global Data" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateGlobalConstants())
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateAttrTable())
                elif "Local Data" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateLocalDefines())
                elif "Local Function Prototypes" in line:
                    # Remove the unused header in the file
                    remove_list.append(index - 1)
                    remove_list.append(index)
                    remove_list.append(index + 1)
                elif "Global Function Definitions" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateTableFunctions())
                elif "Local Function Definitions" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateGetStringFunctions())
                elif "Interrupt Service" in line:
                    # Remove the unused header in the file
                    remove_list.append(index - 1)
                    remove_list.append(index)
                    remove_list.append(index + 1)
            for i in range(len(remove_list)-1, 0, -1):
                # Remove the lines that were saved once the entire file was gone through
                lst.pop(remove_list[i])
            fout.writelines(lst)

    def CreateIds(self) -> str:
        """Create attribute ids for header file"""
        ids = []
        number_ids = len(self.name)
        ids.append("/* pystart - attribute ids */\n")
        for id in range(number_ids):
            result = f"#define ATTR_ID_{self.name[id]}".ljust(
                ID_WIDTH) + str(id) + "\n"
            ids.append(result)
        ids.append("/* pyend */\n\n")
        return ''.join(ids)

    def AddIncludes(self, source_file: bool) -> str:
        """
        Add the other header files need for the project to compile
        """
        includes = []
        includes.append("/* pystart - headers */\n")
        includes.append("#include <zephyr.h>\n")
        if source_file:
            includes.append("#include <string.h>\n")
            includes.append('#include <errno_str.h>\n')
            includes.append('#include "attr_validator.h"\n')
            includes.append('#include "attr_custom_validator.h"\n')
        else:
            includes.append('#include "attr_defs.h"\n')
        includes.append("/* pyend */\n")
        return ''.join(includes)

    def CreateConstants(self) -> str:
        """Create some definitions for header file"""
        defs = []
        maxBinSize = ToInt(max(self.arraySize))
        defs.append("/* pystart - attribute constants */\n")

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
                                       self.EstimateMaxFileSize()))
        defs.append(self.JustifyDefine("ENABLE_FPU_CHECK", "",
                                       self.enableFpuCheck))

        self.CreateMaxStringSizes(defs)
        self.CreateArraySizes(defs)
        defs.append("/* pyend */\n\n")
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

    def EstimateMaxFileSize(self) -> str:
        """
        Return max size of file (without comments)
        Print file with name=<count of max size in chars>\n
        """
        global attr_combine_path
        size = 0
        delimiter_chars = 2
        path = os.path.join(
            attr_combine_path, "max.txt")
        try:
            with open(path, 'w') as f:
                f.write("# Maximum File Size (without comments) for Attributes \n")
                for i in range(self.projectAttributeCount):
                    chars = self.GetMaxChars(i)
                    size += delimiter_chars + len(self.name[i]) + chars
                    f.write(f"{self.name[i]}={CountStr(chars)}\n")
        except:
            print("Unable to open max length file for writing")

        return size

    def MakeDefaultFile(self):
        global attr_combine_path
        path = os.path.join(
            attr_combine_path, "defaults.txt")
        try:
            with open(path, 'w') as f:
                f.write("# Default Values for Attributes \n")
                for i in range(self.projectAttributeCount):
                    default = str(self.default[i])
                    if len(default) == 0:
                        default = EMPTY_VALUE_STRING
                    f.write(f"{self.name[i]}={default}\n")
        except:
            print("Error writing to default attributes file")

    def MakeWritableFile(self):
        global attr_combine_path
        path = os.path.join(
            attr_combine_path, "writable.txt")
        try:
            with open(path, 'w') as f:
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
        lst.append("/* pystart - prepare for read */\n")
        for create, name in zip(self.prepare, self.name):
            if create:
                s = "int attr_prepare_" + name + "(void);\n"

                lst.append(s)
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateEnums(self) -> str:
        lst = []
        lst.append("/* pystart - enumerations */\n")
        GenEnums(lst, self.enum, self.name, self.enum_include_errno)
        # Generator for commands/methods that have enums
        GenEnums(lst, self.methodEnums, self.methodEnumNames,
                 self.methodEnumIncludeErrno)
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateEnumSizeCheck(self) -> str:
        """
        The C-compiler generates the size of the enum based on the number
        elements.
        """
        lst = []
        lst.append("/* pystart - enum size check */\n")
        for i, name in zip(self.enum, self.name):
            if len(i) != 0:
                snake = inflection.underscore(name)
                t = self.GetType(self.name.index(name))
                s = f"BUILD_ASSERT(sizeof(enum {snake}) == "
                s += f"{t.replace('TYPE','SIZE')});\n"
                lst.append(s)
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateGetStringPrototypes(self) -> str:
        """
        Get string function uses integer type so that signature is generic.
        """
        lst = []
        lst.append("/* pystart - get string */\n")
        for enum, name in zip(self.enum, self.name):
            if len(enum) != 0:
                snake = inflection.underscore(name)
                s = f"const char *const attr_get_string_{snake}(int value);\n"
                lst.append(s)
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateTableFunctions(self) -> str:
        """
        The global functions used to verify and setup up attribute table.
        """
        lst = []
        lst.append("/* pystart - attribute functions */\n")
        lst.append("void attr_table_initialize(void)\n")
        lst.append("{\n\tmemcpy(&rw, &DRW, sizeof(rw_attribute_t));\n")
        lst.append("\tmemcpy(&ro, &DRO, sizeof(ro_attribute_t));\n}\n\n")

        lst.append("void attr_table_factory_reset(void)\n")
        lst.append("{\n\tsize_t i = 0;\n")
        lst.append("\tfor (i = 0; i < ATTR_TABLE_SIZE; i++) {\n")
        lst.append("\t\tmemcpy(ATTR_TABLE[i].pData, ATTR_TABLE[i].pDefault,\n")
        lst.append("\t\t\t   ATTR_TABLE[i].size);\n")
        lst.append("\t\t}\n}\n\n")

        lst.append(
            "const struct attr_table_entry *const attr_map(attr_id_t id)\n")
        lst.append("{\n\tif (id > ATTR_TABLE_MAX_ID) {\n")
        lst.append("\t\treturn NULL;\n")
        lst.append("\t} else {\n")
        lst.append("\t\treturn &ATTR_TABLE[id];\n")
        lst.append("\t}\n}\n\n")

        lst.append(
            "attr_index_t attr_table_index(const struct attr_table_entry *const entry)\n")
        lst.append(
            '{\n\t__ASSERT(PART_OF_ARRAY(ATTR_TABLE, entry), "Invalid entry");\n')
        lst.append("\treturn (entry - &ATTR_TABLE[0]);\n")
        lst.append("}\n")
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateGetStringFunctions(self) -> str:
        """Generate functions that return a string of each enum"""
        lst = []
        lst.append("/* pystart - get string */\n")
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
                # Handle special cases
                s = s.replace("Lwm2 M", "LwM2M")
                s = s.replace("Dhcp", "DHCP")
                s = s.replace("Ipv4", "IPv4")
                s = s.replace("Ipv6", "IPv6")
                s = s.replace("2 D", "2D")
                s = s.replace("3 D", "3D")
                s = s.replace("Nb1", "NB1")
                s = s.replace("1 M", "1M")
                s = s.replace("Dc", "DC")
                s = s.replace("Ac", "AC")
                s = s.replace("Poe", "PoE")
                s = s.replace("Psk", "PSK")
                s = s.replace("Rpk", "RPK")
                s = s.replace("Usb", "USB")
                s = s.replace("Wb S1", "WB-S1")
                s = s.replace("Nb S1", "NB-S1")
                lst.append(s)
        lst.append("/* pyend */\n\n")
        return ''.join(lst)

    def CreateAttributeHeaderFile(self, header_file_path: str, lst: list) -> None:
        """Create the attribute header file"""
        print("Writing " + header_file_path)
        with open(header_file_path, 'w') as fout:
            for index, line in enumerate(lst):
                next_line = index + 1
                if "Includes" in line:
                    next_line = next_line + 1
                    lst.insert(next_line,  self.AddIncludes(False))
                elif "Global Data" in line:
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateIds())
                    next_line = next_line + 1
                    lst.insert(next_line, self.CreateConstants())
                elif "Global Function" in line:
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreatePrepare())
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreateGetStringPrototypes())
                elif "Global Constants" in line:
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreateEnums())
                    next_line = next_line + 1
                    lst.insert(
                        next_line, self.CreateEnumSizeCheck())
            fout.writelines(lst)


if __name__ == "__main__":
    file_name_in = "app.yml"
    template_file_name = "template"
    key_names = []
    if (len(sys.argv)-1) < 1:
        raise ValueError(
            'No location entered for the yaml file listing the attribute parameters')
    else:
        for i, arg in enumerate(sys.argv):
            if i == 1:
                location = sys.argv[1]
                # Check for project key
                if (i == (len(sys.argv)-1)):
                    key_names.append("")
            elif i > 1:
                key_names.append(arg)

    # Add index location name to paths
    script_dir = Path(os.path.dirname(__file__))
    component_dir = str(script_dir.parent.parent)
    attr_header_file_path = os.path.normpath(
        attr_header_file_path.replace("%INDEX%", location))
    attr_source_file_path = os.path.normpath(
        attr_source_file_path.replace("%INDEX%", location))
    attr_combine_file_path = os.path.normpath(
        attr_combine_file_path.replace("%INDEX%", location))
    template_file_path = os.path.normpath(
        template_file_path.replace("%COMPONENTS%", component_dir))

    complete_yml_file_name = os.path.join(location, file_name_in)

    complete_attr_source_paths = []
    complete_attr_header_paths = []
    key_file_name = ""
    number_key_names = len(key_names)
    index = 0
    # Change the file path names if key name is used
    while(number_key_names > 0):
        key_file_name = (f"{key_file_name}{key_names[index]}")
        number_key_names = number_key_names - 1
        if(number_key_names > 0):
            index = index + 1
            key_file_name = (f"{key_file_name}_")

    header_path = os.path.normpath(
        attr_header_file_path.replace("%KEY_NAME%", key_file_name))
    source_path = os.path.normpath(
        attr_source_file_path.replace("%KEY_NAME%", key_file_name))
    attr_combine_path = os.path.normpath(
        attr_combine_file_path.replace("%KEY_NAME%", key_file_name))
    complete_attr_header_path = os.path.join(
        header_path, TABLE_FILE_NAME + ".h")
    complete_attr_source_path = os.path.join(
        source_path, TABLE_FILE_NAME + ".c")

    # Ensure .h and .c file exist
    if (not os.path.exists(complete_attr_header_path)):
        try:
            os.makedirs(header_path)
            print("Directory '%s' created" % header_path)
        except:
            pass

        # Copy the file to destination dir
        shutil.copy(os.path.join(template_file_path,
                                 template_file_name + ".h"), complete_attr_header_path)

        copying = True
        dummy_file = complete_attr_header_path + ".bak"
        line_number = 0
        copy_next_line = 0
        with open(complete_attr_header_path, "r") as f, open(dummy_file, 'w') as write_obj:
            for line in f:
                # Need to remove the function prototype comment at the bottom of template file
                if (("/**\n" == line) and (line_number > 0)):
                    copying = False
                elif ((" */\n" == line) and (not copying)):
                    copy_next_line = line_number + 1
                elif (copy_next_line == line_number):
                    copying = True

                if "Remove Empty" in line:
                    # Comment from template header file to be removed
                    line = line.strip(line)
                if "template.h" in line:
                    line = line.replace(template_file_name, TABLE_FILE_NAME)
                if "TEMPLATE" in line:
                    line = line.replace(
                        template_file_name.upper(), TABLE_FILE_NAME.upper())
                if copying:
                    write_obj.write(line)
                line_number = line_number + 1

        os.remove(complete_attr_header_path)
        os.rename(dummy_file, complete_attr_header_path)

    if (not os.path.exists(complete_attr_source_path)):
        try:
            os.makedirs(source_path)
            print("Directory '%s' created" % source_path)
        except:
            pass

        # Copy the file to destination dir
        shutil.copy(os.path.join(template_file_path,
                                 template_file_name + ".c"), complete_attr_source_path)
        with open(complete_attr_source_path, "r") as f:
            source_lines = f.readlines()
            for i, line in enumerate(source_lines):
                if "logging" in line:
                    # Delete logging from the copied template file
                    source_lines[i] = source_lines[i].strip(line)
                if "LOG_LEVEL_DBG" in line:
                    # Delete logging from the copied template file
                    source_lines[i] = source_lines[i].strip(line)
                if "template" in line:
                    source_lines[i] = source_lines[i].replace(
                        template_file_name, TABLE_FILE_NAME)
                if "TEMPLATE" in line:
                    source_lines[i] = source_lines[i].replace(
                        template_file_name.upper(), TABLE_FILE_NAME.upper())
            f.seek(0)
        with open(complete_attr_source_path, "w") as f:
            f.writelines(source_lines)

    CreateCompleteAttributeFile(
        complete_yml_file_name, attr_combine_path, key_names)

    # Parse attributes
    a = attributes(os.path.join(attr_combine_path, DEFAULT_FILE_NAME))

    a.UpdateFiles(complete_attr_header_path, complete_attr_source_path)