""" dkjokes """
import random
import time
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    DkJokes().execute()

    return {
        'message': 'OK'
    }


class DkJokes():
    """ DkJokes backbone. """

    def execute(self):
        """ Run the handler. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f"En vittighed {now}"
        joke = self.get_joke()
        page = self.page(title, joke)
        SendMail().deliver(title, page)


    @staticmethod
    def get_joke():
        """ Read random joke from jokes file. """
        with open('jokes.txt', 'r', encoding='utf8') as jokes_file:
            content = jokes_file.read()

        jokes = content.split('%\n')

        joke = ''
        while joke.strip() == '':
            joke = random.choice(jokes)

        return joke


    @staticmethod
    def page(title, html):
        """ Get HTML page.
            Arguments:
                title - title
                html - body html
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>
<body>
{html}
</body>
</html>"""


if __name__ == "__main__":
    DkJokes().execute()
