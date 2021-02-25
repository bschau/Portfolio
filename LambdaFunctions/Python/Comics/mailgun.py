""" mailgun """
import os
import time
import requests


class MailGun():
    """ MailGun module """

    def __init__(self, module):
        """ Constructor.
            Arguments:
                module: invoking module
        """
        now = time.localtime()
        self.module = module
        self.title = "{0} {1}".format(module,
                                      time.strftime('%Y-%m-%d %H:%M:%S',
                                                    now))


    @staticmethod
    def deliver(title, html):
        """ Deliver email.
            Arguments:
                title - title of mail
                html - html to send
        """
        text = "Please read this in a HTML mail user agent."
        requests.post(
            os.environ['MAILGUN_URL'],
            auth=("api", os.environ['MAILGUN_AUTH']),
            data={"from": os.environ['MAILGUN_FROM'],
                  "to": os.environ['MAILGUN_TO'],
                  "subject": title,
                  "text": text,
                  "html": html
                 },
        )
