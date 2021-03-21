""" sendmail """
import os
import sendgrid
from sendgrid.helpers.mail import Email
from sendgrid.helpers.mail import Mail
from sendgrid.helpers.mail import To
from sendgrid.helpers.mail import Content


class SendMail():
    """ SendMail module """


    @staticmethod
    def deliver(title, body):
        """ Delivery email.
            Arguments:
                title: mail title
                body: body text
        """
        api_key = os.environ["SENDGRID_APIKEY"]
        from_email = Email(os.environ["SENDGRID_FROM"])
        to_email = To(os.environ["SENDGRID_TO"])
        content = Content("text/html", body)
        mail = Mail(from_email, to_email, title, content)
        sendgrid_api = sendgrid.SendGridAPIClient(api_key=api_key)
        sendgrid_api.client.mail.send.post(request_body=mail.get())
