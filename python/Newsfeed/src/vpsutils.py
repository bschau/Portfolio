""" vpsutils """
import configparser
import os
import platform
import smtplib
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import time
import requests


class VpsUtils():
    """ VPS Utilities module """


    def __init__(self, module):
        """ Constructor.
            Arguments:
                module: invoking module
        """
        self.config = self.load_vpsrc()
        self.home = os.path.expanduser("~")
        self.module = module
        if platform.system() == "Windows":
            self.prefix = "_"
            self.tmp = "c:/temp"
        else:
            self.prefix = "."
            self.tmp = "/tmp"

        self.reset_html()


    def reset_html(self):
        """ Reset HTML. """
        self.html = []
        self.now = time.localtime()
        self.title = "{0} {1}".format(self.module,
                                      time.strftime('%Y-%m-%d %H:%M:%S',
                                                    self.now))
        self.html.append("""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>""")
        self.html.append(self.title + """</title>
</head>
<body>""")


    @staticmethod
    def load_vpsrc():
        """ load delivery configuration. """
        home = os.path.expanduser("~")
        vpsrc = configparser.ConfigParser()
        file = "_vpsrc" if platform.system() == 'Windows' else ".vpsrc"
        vpsrc.read(os.path.join(home, file))
        return vpsrc


    def get_config(self, section, key):
        """ Get the value of a key in a section.
            Arguments:
                section: section
                key: key
        """
        return self.config[section][key]


    def deliver(self, att_data=False, att_filename=""):
        """ Delivery email.
        	Arguments:
            	att_data: optional attachment data
                att_filename: optional attachment filename
        """
        self.html.append("</body></html>")
        html = self.html_to_string()

        method = self.config['GENERAL']['method']
        if method == "file":
            path = self.config['FILE']['path']
            with open(path, "w") as html_file:
                html_file.write(html)
            return

        text = "Please read this in a HTML mail user agent."
        if method == "smtp":
            message = MIMEMultipart('alternative')
            message['Subject'] = self.title
            message['From'] = self.config['SMTP']['from']
            message['To'] = self.config['SMTP']['to']
            plain_text = MIMEText(text, 'plain')
            html_text = MIMEText(html, 'html')
            message.attach(plain_text)
            message.attach(html_text)

            if att_data:
                part = MIMEApplication(att_data, att_filename)
                part['Content-Disposition'] = 'attachment; filename="%s"' % att_filename
                message.attach(part)

            mail = smtplib.SMTP('localhost')
            mail.send_message(message)
            mail.quit()
            return

        requests.post(
            self.config['MAILGUN']['url'],
            auth=("api", self.config['MAILGUN']['auth']),
            data={"from": self.config['MAILGUN']['from'],
                  "to": self.config['MAILGUN']['to'],
                  "subject": self.title,
                  "text": text,
                  "html": html
                 },
        )


    def html_to_string(self):
        """ Return HTML buffer as string. """
        return ''.join(self.html)


    def html_add(self, string):
        """ Add string to HTML buffer.
            Arguments:
                string: string with formatted HTML to add
        """
        self.html.append(string)
