import os
import logging
import log_wrapper
import sys
from pathlib import Path

file_name = Path("")

STRING_TO_FIND = '"x-id":'

# Renumber attributes that are greater than equal to base
base = 0
# Offset to apply to renumbering
offset = 0


def renumber(argv):
    """
    Read file and re-number all parameters/attributes.
    This should only be used during initial development.
    Once 'released' the x-id cannot change.
    Does not support commands being in the same file.
    """
    logger = logging.getLogger('renumber')
    lst = []
    count = offset
    with open(file_name, 'r') as fin:
        logger.debug(f"Opened file {file_name}")
        for line in fin:
            default_append = True
            if STRING_TO_FIND in line:
                try:
                    x_id, number = line.split()
                    # Leave room for another (possibly incomplete) attribute list
                    number = number.strip(',')
                    if int(number) >= base:
                        s = x_id + ' ' + str(count) + ',\n'
                        lst.append(s)
                        default_append = False
                        count += 1
                except:
                    logger.debug(f"Couldn't parse: {line.strip()}")

            if default_append:
                lst.append(line)

    if len(lst) > 0:
        with open(file_name, 'w') as fout:
            fout.writelines(lst)
            logger.debug(f"Wrote file {file_name}")

    logger.info(f'{count - offset} total attributes')


if __name__ == "__main__":
    log_wrapper.setup(__file__, console_level=logging.DEBUG, file_mode='a')

    if ((len(sys.argv)-1)) == 1:
        name = sys.argv[1]
        base = 0
        offset = 0
    elif ((len(sys.argv)-1)) == 3:
        name = sys.argv[1]
        base = int(sys.argv[2])
        offset = int(sys.argv[3])
    else:
        name = ""
        raise ValueError('JSON attribute source file must be listed.')

    file_name = Path(name)

    renumber(sys.argv)
