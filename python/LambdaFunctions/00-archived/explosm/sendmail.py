""" sendmail """
import json
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


class SendMail():
    """ SendMail module """

    def __init__(self):
        """ Initialize. """
        with open('settings.json', encoding='utf8') as json_file:
            self.settings = json.load(json_file)


    def deliver(self, title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."

        mailbox = self.settings["mailbox"]
        recipient = self.settings["recipient"]
        message = MIMEMultipart('alternative')
        message['Subject'] = title
        message['From'] = mailbox
        message['To'] = recipient
        message.attach(MIMEText(text, 'plain'))
        message.attach(MIMEText(body, 'html'))

        port = self.settings["port"]
        if port == 25:
            mail = smtplib.SMTP(self.settings["host"], port)
            mail.sendmail(mailbox, recipient, message.as_string())
            mail.quit()
        elif port == 465:
            mail = smtplib.SMTP_SSL(self.settings["host"], port)
            mail.ehlo()
            mail.login(mailbox, self.settings["password"])
            mail.sendmail(mailbox, recipient, message.as_string())
            mail.quit()
        else:
            mail = smtplib.SMTP(self.settings["host"], port)
            mail.ehlo()
            mail.starttls()
            mail.login(mailbox, self.settings["password"])
            mail.sendmail(mailbox, recipient, message.as_string())
            mail.quit()
