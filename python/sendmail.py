""" sendmail """
import configparser
import os
import platform
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import requests
import sendgrid
from sendgrid.helpers.mail import Email
from sendgrid.helpers.mail import Mail
from sendgrid.helpers.mail import To
from sendgrid.helpers.mail import Content


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

        if method == "smtp":
            self.smtp_deliver(title, body)
            return

        if method == "mailgun":
            self.mailgun_deliver(title, body)
            return

        self.sendgrid_deliver(title, body)


    def smtp_deliver(self, title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
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


    def mailgun_deliver(self, title, body):
        """ Delivery email by Sendgrid.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
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


    def sendgrid_deliver(self, title, body):
        """ Delivery email by Sendgrid.
            Arguments:
                title: mail title
                body: body text
        """
        api_key = self.config['SENDGRID']['apikey']
        from_email = Email(self.config['SMTP']['from'])
        to_email = To(self.config['SMTP']['to'])
        content = Content("text/html", body)
        mail = Mail(from_email, to_email, title, content)
        sendgrid_api = sendgrid.SendGridAPIClient(api_key=api_key)
        sendgrid_api.client.mail.send.post(request_body=mail.get())


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
