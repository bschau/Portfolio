""" sendmail """
import configparser
import os
import platform
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import requests


class SendMail():
    """ SendMail module """

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


    def deliver(self, title, body):
        """ Delivery email.
            Arguments:
                title: mail title
                body: body text
        """
        method = self.config['GENERAL']['method']
        if method == "file":
            path = self.config['FILE']['path']
            with open(path, "w") as html_file:
                html_file.write(body)
            return

        text = "Please read this in a HTML mail user agent."
        if method == "smtp":
            recipients = self.get_recipients('SMTP')
            message = MIMEMultipart('alternative')
            message['Subject'] = title
            message['From'] = self.config['SMTP']['from']
            message['To'] = ", ".join(recipients)
            plain_text = MIMEText(text, 'plain')
            html_text = MIMEText(body, 'html')
            message.attach(plain_text)
            message.attach(html_text)

            mail = smtplib.SMTP('localhost')
            mail.sendmail(message['From'], recipients, message.as_string())
            mail.quit()
            return

        requests.post(
            self.config['MAILGUN']['url'],
            auth=("api", self.config['MAILGUN']['auth']),
            data={"from": self.config['MAILGUN']['from'],
                  "to": self.get_recipients('MAILGUN'),
                  "subject": title,
                  "text": text,
                  "html": body
                 },
        )


    def get_recipients(self, section):
        """ Get the list of recipients.
            Arguments:
                section: major section for initial recipient.
        """
        recipients = []
        for recipient in self.config[section]['to'].split(','):
            recipients.append(recipient.strip())

        if self.module not in self.config.sections():
            return recipients

        for recipient in self.config[self.module]['additional_to'].split(','):
            recipients.append(recipient.strip())

        return recipients
