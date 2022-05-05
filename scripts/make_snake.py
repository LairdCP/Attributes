import os
import logging
import log_wrapper
import sys
import inflection
from pathlib import Path

file_name = Path("")

STRING_TO_FIND = '"name":'


def make_snake(argv):
    logger = logging.getLogger('make_snake')
    lst = []
    with open(file_name, 'r') as fin:
        logger.debug(f"Opened file {file_name}")
        for line in fin:
            default_append = True
            if STRING_TO_FIND in line:
                try:
                    name_field, name = line.split()
                    name = name.strip(',').strip('"')
                    #logger.debug(f"{name} -> {inflection.underscore(name)}")
                    s = name_field + '"' + inflection.underscore(name) + '",\n'
                    lst.append(s)
                    default_append = False
                except:
                    logger.debug(f"Couldn't parse: {line.strip()}")

            if default_append:
                lst.append(line)

    if len(lst) > 0:
        with open(file_name, 'w') as fout:
            fout.writelines(lst)
            logger.debug(f"Wrote file {file_name}")


if __name__ == "__main__":
    log_wrapper.setup(__file__, console_level=logging.DEBUG, file_mode='a')

    if ((len(sys.argv)-1)) == 1:
        name = sys.argv[1]
    else:
        name = ""
        raise ValueError('JSON attribute source file must be listed.')

    file_name = Path(name)

    make_snake(sys.argv)
