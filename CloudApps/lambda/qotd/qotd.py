""" qotd """
import random
import time
from mailgun import MailGun


class Qotd():
    """ Qotd backbone. """

    def execute(self):
        """ Qotd backbone handler. """
        title = self.title()
        page = self.page(title)
        qotd = self.get_qotd()
        qotd = self.parse_qotd(qotd)
        qotd = '<p>' + qotd + '</p>'
        MailGun('Qotd').deliver(title, page.format(qotd))


    @staticmethod
    def title():
        """ Get title of mail. """
        return "Qotd {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                               time.localtime()))


    @staticmethod
    def page(title):
        """ Get HTML page.
            Arguments:
                title - title.
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
</html>""".format(title)


    @staticmethod
    def get_qotd():
        """ Read random quote from quote file. """
        with open("sayings.txt", 'r') as qotds_file:
            content = qotds_file.read()

        qotds = content.split('%\n')

        qotd = ''
        while qotd.strip() == '':
            qotd = random.choice(qotds)

        return qotd


    @staticmethod
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
