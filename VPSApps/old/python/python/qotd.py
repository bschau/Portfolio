""" qotd """
import random
import os
import platform
from vpsutils import VpsUtils


def get_qotd(vps):
    """ Read random quote from quote file.
    	Arguments:
        	vps. VpsUtils
    """
    name = "_qotd" if platform.system() == 'Windows' else ".qotd"
    qotd_file = os.path.join(vps.home, name)

    with open(qotd_file, 'r') as qotds_file:
        content = qotds_file.read()

    qotds = content.split('%\n')

    qotd = ''
    while qotd.strip() == '':
        qotd = random.choice(qotds)

    return qotd


def main():
    """ Main entry method. """
    vps = VpsUtils('Quote of the Day')
    qotd = get_qotd(vps)

    index = 0
    str_len = len(qotd)
    in_bold = False
    in_italic = False
    while index < str_len:
        character = qotd[index]
        if character == '\\':
            index = index + 1
            vps.html_add(qotd[index])
        elif character == '\n':
            vps.html_add('<br />')
        elif character == '*':
            vps.html_add('</b>' if in_bold else '<b>')
            in_bold = not in_bold
        elif character == '_':
            vps.html_add('</i>' if in_italic else '<i>')
            in_italic = not in_italic
        elif character == '#':
            vps.html_add('&nbsp;')
        else:
            vps.html_add(character)

        index = index + 1

    vps.deliver()


if __name__ == '__main__':
    main()
