#
# @file attribute_generator.py
#
# @brief Generate C code from JSON API document.
#
# Copyright (c) 2018-2022 Laird Connectivity
#
# SPDX-License-Identifier: Apache-2.0
#
from importlib.resources import path
import json
import jsonref
import collections
import os
import inflection
import sys
import math
import shutil
from pathlib import Path

JSON_INDENT = '  '

# Left Justified Field Widths
ID_WIDTH = 54
NAME_MACRO_WIDTH = 40
AV_FIELD_WIDTH = 20
AP_WIDTH = 36
DEFINE_WIDTH = 20
GS_CASE_WIDTH = 18
TYPE_WIDTH = 24
MIN_MAX_WIDTH = 20

header_file_path = Path("")
source_file_path = Path("")


def PathAndTemplateHandler(json_file_name):
    """
    Build header and source file paths based on location of attributes.json.
    If required, make destination folders.
    If required, copy template files from library.
    """
    global header_file_path
    global source_file_path
    # Add folder name to path
    # Ensure directories used for generation exist
    #
    # Project Folder Structure
    # ./attributes.json
    # ./include/attr_table.h
    # ./source/attr_table.c
    #
    local_path = Path(os.path.dirname(json_file_name))
    header_file_path = local_path.joinpath("include")
    source_file_path = local_path.joinpath("source")
    print(f"source dir: {local_path}")
    if (not os.path.isdir(header_file_path)):
        os.mkdir(header_file_path)
        print("Created header folder for project")
    if (not os.path.isdir(source_file_path)):
        os.mkdir(source_file_path)
        print("Created source folder for project")

    # Add file name to path
    # If .h and .c file don't exist, then copy them from template folder
    #
    # Attribute Library Folder Structure
    # ./scripts/*.py
    # ./template/attr_table.h
    # ./template/attr_table.c
    #
    header_file_path = header_file_path.joinpath("attr_table.h")
    source_file_path = source_file_path.joinpath("attr_table.c")
    script_parent_dir = Path(os.path.dirname(__file__)).parent
    print(f"script dir: {script_parent_dir}")
    if (not os.path.exists(header_file_path)):
        shutil.copyfile(script_parent_dir.joinpath(
            "template", "include", "attr_table.h"), header_file_path)
    if (not os.path.exists(source_file_path)):
        shutil.copyfile(script_parent_dir.joinpath(
            "template", "source", "attr_table.c"), source_file_path)


def ToYesNo(b) -> str:
    if b:
        return "y"
    else:
        return "n"


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


class attributes:
    def __init__(self, fname):

        # The following items are loaded from the configuration file
        self.parameterList = 0
        self.projectAttributeCount = 0
        self.apiTotalAttributes = 0
        self.maxNameLength = 0
        self.largestNumberSize = 0

        self.id = []
        self.apiId = []
        self.max = []
        self.min = []
        self.name = []
        self.apiName = []
        self.type = []
        self.lockable = []
        self.broadcast = []
        self.stringMax = []
        self.arraySize = []
        self.default = []
        self.writable = []
        self.readable = []
        self.savable = []
        self.deprecated = []
        self.validator = []
        self.prepare = []
        self.enum = []
        self.enum_include_errno = []
        self.methodEnums = []
        self.methodEnumNames = []
        self.methodEnumIncludeErrno = []

        self.IncrementVersion(fname)
        self.LoadConfig(fname)

    def IncrementVersion(self, fname) -> None:
        """
        Increment version of the form x.y.z and write it back to the file.
        """
        with open(fname, 'r') as f:
            data = json.load(f)
            major, minor, build = data['info']['version'].split('.')
            build = int(build) + 1
            new_version = f'{major}.{minor}.{build}'
            data['info']['version'] = new_version

            plist = data['components']['contentDescriptors']['deviceParams']['x-device-parameters']
            for p in plist:
                if p['name'] == "attribute_version":
                    p['schema']['x-default'] = new_version
                    with open(fname, 'w') as f:
                        json.dump(data, f, indent=JSON_INDENT)
                    print(new_version)
                    return

            print("Unable to write api version")

    def remove_decimal_point_from_integers(self, fname) -> None:
        """
        The PC test tool previously required all min/max values to be floats.
        This is no longer the case and decimal points can be removed.
        """
        with open(fname, 'r') as f:
            data = jsonref.load(f)
            self.parameterList = data['components']['contentDescriptors']['deviceParams']['x-device-parameters']

            for p in self.parameterList:
                a = p['schema']
                if 'float' not in a['x-ctype']:
                    try:
                        x = a['maximum']
                        a['maximum'] = int(x)
                    except:
                        pass
                    try:
                        x = a['minimum']
                        a['minimum'] = int(x)
                    except:
                        pass

        with open(fname, 'w') as f:
            json.dump(data, f, indent=JSON_INDENT)

    def LoadConfig(self, fname) -> None:
        with open(fname, 'r') as f:
            data = jsonref.load(f)
            self.parameterList = data['components']['contentDescriptors']['deviceParams']['x-device-parameters']
            self.apiTotalAttributes = len(self.parameterList)

            # Extract the properties for each parameter
            for p in self.parameterList:
                self.apiName.append(p['name'])
                self.apiId.append(p['x-id'])
                # required fields
                self.name.append(p['name'])
                self.id.append(p['x-id'])
                # required schema fields
                a = p['schema']
                self.default.append(a['x-default'])
                self.type.append(a['x-ctype'])
                # optional schema fields have a default value
                self.arraySize.append(GetNumberField(a, 'x-array-size'))
                self.max.append(GetNumberField(a, 'maximum'))
                self.min.append(GetNumberField(a, 'minimum'))
                self.lockable.append(GetBoolField(a, 'x-lockable'))
                self.broadcast.append(GetBoolField(a, 'x-broadcast'))
                self.readable.append(GetBoolField(a, 'x-readable'))
                self.writable.append(GetBoolField(a, 'x-writable'))
                self.savable.append(GetBoolField(a, 'x-savable'))
                self.deprecated.append(GetBoolField(a, 'x-deprecated'))
                self.validator.append(GetStringField(a, 'x-validator'))
                self.prepare.append(GetBoolField(a, 'x-prepare'))
                self.enum.append(GetDictionaryField(a, 'enum'))
                self.enum_include_errno.append(
                    GetBoolField(a, 'x-enum-include-errno'))
                # Max string size is only required for strings.
                # Min and max are lengths when type is a string.
                if a['x-ctype'] == "string":
                    self.stringMax.append(GetNumberField(a, 'maximum'))
                else:
                    self.stringMax.append(0)

            self.projectAttributeCount = len(self.name)
            print(f"API Total Attributes {self.apiTotalAttributes}")
            print(
                f"Project Attributes {self.projectAttributeCount}")
            print(f"Project Maximum ID {max(self.id)}")
            self.PrintAvailableIds()
            pass

    def GetType(self, index: int) -> str:
        kind = self.type[index]
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
        else:
            s += "UNKNOWN"

        if (numberSize > self.largestNumberSize):
            self.largestNumberSize = numberSize

        return s

    def GetAttributeMacro(self, index: int) -> str:
        """Get the c-macro for the RW or RO attribute (array vs non-array pointer)"""
        kind = self.type[index]
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
        kind = self.type[index]
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
        kind = self.type[index]
        i_min = self.min[index]
        i_max = self.max[index]
        if kind == "float":
            s_min = ".min.fx = " + str(i_min)
            s_max = ".max.fx = " + str(i_max)
        else:
            i_min = int(i_min)
            i_max = int(i_max)
            if i_min < 0 or i_max < 0:
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
        created from the Excel spreadsheet and gperf
        """
        attributeTable = []
        for i in range(self.projectAttributeCount):
            writable = ToYesNo(self.writable[i])
            readable = ToYesNo(self.readable[i])
            lockable = ToYesNo(self.lockable[i])
            broadcast = ToYesNo(self.broadcast[i])
            savable = ToYesNo(self.savable[i])
            deprecated = ToYesNo(self.deprecated[i])
            result = f"\t[{i:<3}] = " \
                + "{ " + f"{self.id[i]:<3}, " \
                + f"{self.GetAttributeMacro(i)}, {self.GetType(i).ljust(TYPE_WIDTH)}, {savable}, " \
                + f"{writable}, {readable}, {lockable}, {broadcast}, {deprecated}, " \
                + f"{self.GetValidatorString(i)}, {self.GetPrepareString(i)}, " \
                + f"{self.CreateMinMaxString(i)}" \
                + " }," \
                + "\n"
            attributeTable.append(result)

        attributeTable.append("\n")

        string = ''.join(attributeTable)
        return string[:string.rfind(',')] + '\n'

    def GenerateVariable(self, kind: str, maximum: int) -> str:
        """ Determine if variable is an string/array """
        if kind == "char":
            # add one for the NUL character
            return f"[{maximum} + 1]"
        elif maximum != 0:
            # this is an array (byte, float, ...)
            return f"[{maximum}]"
        else:
            return ""

    def GetDefault(self, kind: str, default) -> str:
        if default == "NA":
            if kind == "char":
                return '""'
            elif kind == "float":
                return str(0.0)
            else:
                return str(0)
        else:
            if kind == "char":
                return ('"' + default + '"')
            elif type(default) == bool:
                if default:
                    return "true"
                else:
                    return "false"
            else:
                return str(default)

    def GetPrepareString(self, index: int) -> str:
        name = self.name[index]
        if self.prepare[index]:
            s = "attr_prepare_" + name
        else:
            s = "NULL"

        return s.ljust(AP_WIDTH)

    def CheckForDuplicates(self) -> bool:
        """
        Check for duplicate parameter IDs or names.
        """
        if len(set(self.id)) != len(self.id):
            print("Duplicate attribute ID in Project")
            PrintDuplicate(self.apiName)
            return False

        if len(set(self.apiId)) != len(self.apiId):
            print("Duplicate attribute ID in API")
            PrintDuplicate(self.apiName)
            return False

        if len(set(self.name)) != len(self.name):
            print("Duplicate Attribute Name")
            PrintDuplicate(self.apiName)
            return False

        if len(set(self.apiName)) != len(self.apiName):
            print("Duplicate Attribute Name in API")
            PrintDuplicate(self.apiName)
            return False

        return True

    def UpdateFiles(self) -> None:
        """
        Update the attribute c/h files.
        """
        self.CheckForDuplicates()
        self.CreateSourceFile(
            self.CreateInsertionList(source_file_path))
        self._CreateAttributeHeaderFile(
            self.CreateInsertionList(header_file_path))

    def CreateInsertionList(self, name) -> list:
        """
        Read in the c/h file and create a list of strings that
        is ready for the attribute information to be inserted
        """
        print(f"Reading {name}")
        lst = []
        with open(name, 'r') as fin:
            copying = True
            for line in fin:
                if "pystart" in line:
                    lst.append(line)
                    copying = False
                elif "pyend" in line:
                    lst.append(line)
                    copying = True
                elif copying:
                    lst.append(line)

        return lst

    def CreateStruct(self, category: str, default_values: bool, remove_last_comma: bool) -> str:
        """
        Creates the structures and default values for RW and RO attributes.
        Writable but non-savable values are populated in the RO structure.
        """
        struct = []
        for i in range(self.projectAttributeCount):
            savable = self.savable[i]
            if (category == 'rw' and savable) or (
                    (category == 'ro') and not savable):
                name = self.name[i]
                # string is required in test tool, c requires char type
                if self.type[i] == "string":
                    kind = "char"
                    i_max = self.stringMax[i]
                elif len(self.enum[i]) != 0:
                    snake = inflection.underscore(self.name[i])
                    kind = f"enum {snake}"
                    i_max = self.arraySize[i]
                else:
                    kind = self.type[i]
                    i_max = self.arraySize[i]

                # Use tabs because we use tabs with Zephyr/clang-format.
                if default_values:
                    result = f"\t.{name} = {self.GetDefault(kind, self.default[i])}," + "\n"
                else:
                    result = f"\t{kind} {name}{self.GenerateVariable(kind, i_max)};" + "\n"
                struct.append(result)

        string = ''.join(struct)
        if default_values and remove_last_comma:
            return string[:string.rfind(',')] + '\n'
        else:
            return string

    def CreateMap(self) -> str:
        """
        Create map of ids to table entries
        Invalid entries are NULL
        """
        s = ""
        for i in range(max(self.id) + 1):
            if i in self.id:
                idx = self.id.index(i)
                s += f"\t[{i:<3}] = &ATTR_TABLE[{idx:<3}],\n"

        s = s[:s.rfind(',')] + '\n'

        return s

    def PrintAvailableIds(self):
        available = []
        for i in range(self.apiTotalAttributes):
            if i not in self.apiId:
                available.append(i)

        print(f"Available API IDs\n {available}")

    def CreateSourceFile(self, lst: list) -> None:
        """Create the settings/attributes/properties *.c file"""
        name = source_file_path
        print(f"Writing {name}")
        with open(name, 'w') as fout:
            for index, line in enumerate(lst):
                next_line = index + 1
                if "pystart - " in line:
                    if "attribute table" in line:
                        lst.insert(next_line, self.CreateAttrTable())
                    elif "attribute map" in line:
                        lst.insert(next_line, self.CreateMap())
                    elif "rw attributes" in line:
                        lst.insert(
                            next_line, self.CreateStruct("rw", False, False))
                    elif "rw defaults" in line:
                        lst.insert(
                            next_line, self.CreateStruct("rw", True, True))
                    elif "ro attributes" in line:
                        lst.insert(
                            next_line, self.CreateStruct("ro", False, True))
                    elif "ro defaults" in line:
                        lst.insert(
                            next_line, self.CreateStruct("ro", True, True))
                    elif "prepare for read - weak implementations" in line:
                        lst.insert(next_line, self.CreatePrepare(False))
                    elif "get string" in line:
                        lst.insert(next_line, self.CreateGetStringFunctions())

            fout.writelines(lst)

    def CreateIds(self) -> str:
        """Create attribute ids for header file"""
        ids = []
        for name, id in zip(self.name, self.id):
            result = f"#define ATTR_ID_{name}".ljust(ID_WIDTH) + str(id) + "\n"
            ids.append(result)
        return ''.join(ids)

    def CreateIndices(self) -> str:
        """Create attribute indices for header file"""
        special = ["lock", "load_path"]
        indices = []
        for i in range(self.projectAttributeCount):
            name = self.name[i]
            if name in special:
                result = f"#define ATTR_INDEX_{name:<34} {i}" + "\n"
                indices.append(result)
        return ''.join(indices)

    def CreateConstants(self) -> str:
        """Create some definitions for header file"""
        defs = []
        maxBinSize = ToInt(max(self.arraySize))

        if (self.largestNumberSize > maxBinSize):
            maxBinSize = self.largestNumberSize

        self.maxNameLength = len(max(self.name, key=len))

        defs.append(self.JustifyDefine(
            "TABLE_SIZE", "", self.projectAttributeCount))
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
        """Create max string size definitions (size includes NULL terminator)"""
        lst.append("\n")
        lst.append("/* Attribute Max String Lengths */\n")
        for kind, length, name in zip(self.type, self.stringMax, self.name):
            if kind == "string":
                snake = inflection.underscore(name).upper()
                lst.append(self.JustifyDefine(
                    snake, "MAX_STR_SIZE", length + 1))

        return ''.join(lst)

    def CreateArraySizes(self, lst: list) -> str:
        """Create array size definitions (only byte arrays are currently supported)"""
        lst.append("\n")
        lst.append("/* Attribute Byte Array Lengths */\n")
        for size, name in zip(self.arraySize, self.name):
            if size != 0:
                name = inflection.underscore(name).upper()
                lst.append(self.JustifyDefine(name, "SIZE", size))

        return ''.join(lst)

    def GeneratePrepareFunctionName(self, index: int) -> str:
        return

    def CreatePrepare(self, prototype: bool) -> str:
        """Create prototypes or weak implementation for prepare to read functions"""
        lst = []
        for create, name in zip(self.prepare, self.name):
            if create:
                s = "int attr_prepare_" + name + "(void)"
                if prototype:
                    s += ";\n"
                else:
                    s = "__weak " + s + "\n{\n\treturn 0;\n}\n\n"

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

    def CreateGetStringProtypes(self) -> str:
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
                # Handle special cases
                s = s.replace("Lwm2 M", "LwM2M")
                s = s.replace("Dhcp", "DHCP")
                s = s.replace("Ipv4", "IPv4")
                s = s.replace("Ipv6", "IPv6")
                s = s.replace("2 D", "2D")
                s = s.replace("3 D", "3D")
                s = s.replace("Nb1", "NB1")
                s = s.replace("1 M", "1M")
                s = s.replace("Wb S1", "WB-S1")
                s = s.replace("Nb S1", "NB-S1")
                lst.append(s)

        return ''.join(lst)

    def _CreateAttributeHeaderFile(self, lst: list) -> None:
        """Create the attribute header file"""
        name = header_file_path
        print(f"Writing {name}")
        with open(name, 'w') as fout:
            for index, line in enumerate(lst):
                next_line = index + 1
                if "pystart - " in line:
                    if "attribute ids" in line:
                        lst.insert(next_line, self.CreateIds())
                    elif "attribute indices" in line:
                        lst.insert(next_line, self.CreateIndices())
                    elif "attribute constants" in line:
                        lst.insert(next_line, self.CreateConstants())
                    elif "prepare for read" in line:
                        lst.insert(
                            next_line, self.CreatePrepare(True))
                    elif "enumerations" in line:
                        lst.insert(
                            next_line, self.CreateEnums())
                    elif "enum size check" in line:
                        lst.insert(
                            next_line, self.CreateEnumSizeCheck())
                    elif "get string" in line:
                        lst.insert(
                            next_line, self.CreateGetStringProtypes())

            fout.writelines(lst)


if __name__ == "__main__":
    if ((len(sys.argv)-1)) == 1:
        json_file = sys.argv[1]
    else:
        json_file = ""
        raise ValueError('JSON attribute source file must be listed.')

    PathAndTemplateHandler(json_file)

    # The header/source file paths are globals.
    # They are used when generating files.
    a = attributes(json_file)

    a.UpdateFiles()
