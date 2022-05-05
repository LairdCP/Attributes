#
# @file remove_projects.py
#
# @brief Remove projects from a attributes.json file
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


def remove_projects(fname) -> None:
    """ Remove item if project doesn't match and remove x-projects key"""
    with open(fname, 'r') as f:
        data = jsonref.load(f)

        # Use a removal list as an alternative to creating a second copy
        plist = data['components']['contentDescriptors']['deviceParams']['x-device-parameters']
        non_project_list = list()
        for p in plist:
            try:
                if project:
                    if project not in p['x-projects']:
                        non_project_list.append(p)
                p.pop('x-projects', None)
                p.pop('required', None)
            except:
                pass

        for x in non_project_list:
            plist.remove(x)

        with open(fname, 'w') as f:
            json.dump(data, f, indent=JSON_INDENT)


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

    remove_projects(file_name)
