#
# @file remote_quotes.py
#
# @brief Remove quotes from items that aren't strings
#
# Copyright (c) 2022 Laird Connectivity
#
# SPDX-License-Identifier: Apache-2.0
#
from importlib.resources import path
import json
import jsonref
import collections
import os
import sys
from pathlib import Path

JSON_INDENT = '  '

project = str("")


def remote_quotes(fname) -> None:
    """ Remove quotes from items that aren't strings"""
    with open(fname, 'r') as f:
        data = jsonref.load(f)

        count = 0
        plist = data['components']['contentDescriptors']['deviceParams']['x-device-parameters']
        for p in plist:
            if p['schema']['x-ctype'] != "string" and p['schema']['x-ctype'] != "array":
                for key in ('x-default', 'x-example'):
                    try:
                        val = p['schema'][key]
                        if type(val) == str:
                            count += 1
                            if p['schema']['x-ctype'] == "float":
                                p['schema'][key] = float(val)
                            elif p['schema']['x-ctype'] == "bool":
                                # dump will convert Python True to JSON true
                                if val == "true":
                                    p['schema'][key] = True
                                else:
                                    p['schema'][key] = False
                            else:
                                p['schema'][key] = int(val)
                    except:
                        continue

        with open(fname, 'w') as f:
            json.dump(data, f, indent=JSON_INDENT)

            print(f"Modified {count} entries")


if __name__ == "__main__":
    if ((len(sys.argv)-1)) == 1:
        name = str(sys.argv[1])
        project = ""
    elif ((len(sys.argv)-1)) == 2:
        name = str(sys.argv[1])
        project = str(sys.argv[2])
    else:
        name = ""
        raise ValueError('JSON attribute source file must be listed.')

    file_name = Path(name)

    remote_quotes(file_name)
