""" todo.py

Requirements: htmlentities, requests
"""
import smtplib
import os
import platform
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from datetime import datetime
import htmlentities


class Todo():
    """ Todo backbone. """

    def __init__(self):
        """ Initialize class. """
        self.now = datetime.today()


    def execute(self):
        """ Events backbone handler. """
        title = self.title()
        page = self.head(title)
        todos = htmlentities.encode(self.get_todos()).replace("\n", "<br />")
        page = f'{page}<body>{todos}</body></html>'
        self.deliver(title, page)


    @staticmethod
    def title():
        """ Get title of mail. """
        now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        return f"Todo {now}"


    @staticmethod
    def head(title):
        """ Get HTML head (of page).
            Arguments:
                title - title.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>"""


    @staticmethod
    def get_todos():
        """ Get the content of the TODO file. """
        file = '_todorc' if platform.system() == 'Windows' else '.todorc'
        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        with open(full_path, "r", encoding='utf8') as todo_file:
            return todo_file.read()


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["bschau@posteo.net"]
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


if __name__ == '__main__':
    Todo().execute()
