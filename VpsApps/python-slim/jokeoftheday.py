""" dkjokes """
import random
import time
import os
import platform
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


class JokeOfTheDay():
    """ JokeOfTheDay backbone. """

    def execute(self):
        """ Run the handler. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f"Joke of the Day {now}"
        joke = self.parse_joke(self.get_joke())
        page = self.page(title, joke)
        self.deliver(title, page)


    @staticmethod
    def get_joke():
        """ Read random joke from jokes file. """
        file = '_jokes' if platform.system() == 'Windows' else '.jokes'
        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        with open(full_path, 'r', encoding='utf8') as jokes_file:
            content = jokes_file.read()

        jokes = content.split('%\n')

        joke = ''
        while joke.strip() == '':
            joke = random.choice(jokes)

        return joke


    @staticmethod
    def parse_joke(raw):
        """ Parse the joke.
            Arguments:
                raw - raw joke.
        """
        index = 0
        str_len = len(raw)
        in_bold = False
        in_italic = False
        out = ""
        while index < str_len:
            character = raw[index]
            if character == '\\':
                index = index + 1
                out = out + raw[index]
            elif character == '*':
                tag = '</b>' if in_bold else '<b>'
                out = out + tag
                in_bold = not in_bold
            elif character == '_':
                tag = '</i>' if in_italic else '<i>'
                out = out + tag
                in_italic = not in_italic
            elif character == '#':
                out = out + '&nbsp;'
            elif character == '\n':
                out = out + '<br />'
            else:
                if character != '\r':
                    out = out + character

            index = index + 1

        return out


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
<p>
{html}
</p>
</body>
</html>"""


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian@schau.dk"]
        message = MIMEMultipart('alternative')
        message['Subject'] = title
        message['From'] = "bs@leah.schau.dk"
        message['To'] = ",".join(recipients)
        plain_text = MIMEText(text, 'plain')
        html_text = MIMEText(body, 'html')
        message.attach(plain_text)
        message.attach(html_text)

        mail = smtplib.SMTP('localhost')
        mail.sendmail(message['From'], recipients, message.as_string())
        mail.quit()


if __name__ == "__main__":
    JokeOfTheDay().execute()
