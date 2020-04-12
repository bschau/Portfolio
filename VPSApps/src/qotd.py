""" qotd """
import os
import platform
import random
from vpsutils import VpsUtils


def get_qotd(vps):
    """ Read random quote from quote file.
        Arguments:
            vps: VpsUtils
    """
    file = '_sayings' if platform.system() == 'Windows' else '.sayings'
    full_path = os.path.join(vps.home, file)

    with open(full_path, 'r') as qotds_file:
        content = qotds_file.read()

    qotds = content.split('%\n')

    qotd = ''
    while qotd.strip() == '':
        qotd = random.choice(qotds)

    return qotd


def parse_qotd(qotd):
    """ Parse the quote.
        Arguments:
            qotd - random quote.
    """
    index = 0
    str_len = len(qotd)
    in_bold = False
    in_italic = False
    html = ""
    while index < str_len:
        character = qotd[index]
        if character == '\\':
            index = index + 1
            html = html + qotd[index]
        elif character == '\n':
            html = html + '<br />'
        elif character == '*':
            tag = '</b>' if in_bold else '<b>'
            html = html + tag
            in_bold = not in_bold
        elif character == '_':
            tag = '</i>' if in_italic else '<i>'
            html = html + tag
            in_italic = not in_italic
        elif character == '#':
            html = html + '&nbsp;'
        else:
            html = html + character

        index = index + 1

    return html


def main():
    """ Main entry method. """
    vps = VpsUtils('Qotd')
    qotd = get_qotd(vps)
    qotd = parse_qotd(qotd)
    qotd = '<p>' + qotd + '</p>'
    vps.html_add(qotd)
    vps.deliver()


if __name__ == '__main__':
    main()
