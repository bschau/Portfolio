""" dkjokes """
import os
import platform
import random
import time
from sendmail import SendMail


class DkJokes():
    """ DkJokes backbone. """

    def execute(self):
        """ Run the handler. """
        joke = self.get_joke()
        title = self.title()
        page = self.page(title, joke)
        SendMail('DkJokes').deliver(title, page)


    @staticmethod
    def get_jokes_file():
        """ Get location of jokes file. """
        home = os.path.expanduser("~")
        file = "_dkjokes" if platform.system() == 'Windows' else ".dkjokes"
        return os.path.join(home, file)


    def get_joke(self):
        """ Read random joke from jokes file. """
        file = self.get_jokes_file()
        with open(file, 'r') as jokes_file:
            content = jokes_file.read()

        jokes = content.split('%\n')

        joke = ''
        while joke.strip() == '':
            joke = random.choice(jokes)

        return joke


    @staticmethod
    def title():
        """ Get title of mail. """
        return "En vittighed {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                                       time.localtime()))


    @staticmethod
    def page(title, html):
        """ Get HTML page.
            Arguments:
                title - title
                html - body html
        """
        return """<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{0}</title>
</head>
<body>
{1}
</body>
</html>""".format(title, html)


if __name__ == "__main__":
    DkJokes().execute()
