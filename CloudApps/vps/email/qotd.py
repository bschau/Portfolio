""" qotd """
import os
import platform
import random
import time
from vpsutils import VpsUtils


def get_qotd_file(vps):
    """ Get location of Quote of the Day file.
    	Args:
        	vps: VpsUtils
    """
    file = "_qotd" if platform.system() == 'Windows' else ".qotd"
    return os.path.join(vps.home, file)


def title():
    """ Get title of mail. """
    return "Qotd {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                           time.localtime()))


def page(page_title):
    """ Get HTML page.
        Arguments:
            page_title - title.
    """
    return """<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{0}</title>
</head>
<body>
{{0}}
</body>
</html>""".format(page_title)


def get_qotd(vps):
    """ Read random quote from quote file.
        Args:
            vps: VpsUtils
    """
    file = get_qotd_file(vps)
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
    """ Main loop. """
    vps = VpsUtils('Quote of the Day')
    html = page(title())
    qotd = get_qotd(vps)
    qotd = parse_qotd(qotd)
    qotd = '<p>' + qotd + '</p>'
    html = html.format(qotd)
    vps.html_add(html)
    vps.deliver()


if __name__ == "__main__":
    main()
