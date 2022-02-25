import os
import logging
import log_wrapper
import sys
import inflection

FILE_NAME = "./attributes.json"

STRING_TO_FIND = '"name":'

BASE = 140
OFFSET = 150


def make_snake(argv):
    logger = logging.getLogger('make_snake')
    lst = []
    count = OFFSET
    with open(FILE_NAME, 'r') as fin:
        logger.debug("Opened file " + FILE_NAME)
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
        with open(FILE_NAME, 'w') as fout:
            fout.writelines(lst)
            logger.debug("Wrote file " + FILE_NAME)


if __name__ == "__main__":
    log_wrapper.setup(__file__, console_level=logging.DEBUG, file_mode='a')
    make_snake(sys.argv)
