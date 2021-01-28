""" qotd """
import os
import platform
import random


def get_qotd_file():
    """ Get location of Quote of the Day file. """
    home = os.path.expanduser("~")
    file = "_qotd" if platform.system() == 'Windows' else ".qotd"
    return os.path.join(home, file)


def get_qotd():
    """ Read random quote from quote file. """
    file = get_qotd_file()
    with open(file, 'r') as qotds_file:
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
    out = ""
    while index < str_len:
        character = qotd[index]
        if character == '\\':
            index = index + 1
            out = out + qotd[index]
        elif character == '*':
            tag = '\033[0m' if in_bold else '\033[1m'
            out = out + tag
            in_bold = not in_bold
        elif character == '_':
            tag = '\033[0m' if in_italic else '\033[7m'
            out = out + tag
            in_italic = not in_italic
        elif character == '#':
            out = out + ' '
        else:
            out = out + character

        index = index + 1

    return out


def main():
    """ Main loop. """
    qotd = get_qotd()
    print(parse_qotd(qotd))


if __name__ == "__main__":
    main()
