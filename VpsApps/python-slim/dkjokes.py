""" dkjokes """
import random
import time
import os
import platform
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


class DkJokes():
    """ DkJokes backbone. """

    def execute(self):
        """ Run the handler. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f"En vittighed {now}"
        joke = self.get_joke()
        page = self.page(title, joke)
        self.deliver(title, page)


    @staticmethod
    def get_joke():
        """ Read random joke from jokes file. """
        file = '_dkjokes' if platform.system() == 'Windows' else '.dkjokes'
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


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian.schau@gmail.com"]
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
    DkJokes().execute()
