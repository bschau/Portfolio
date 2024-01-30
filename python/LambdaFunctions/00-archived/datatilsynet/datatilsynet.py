""" datatilsynet

Requirements: feedparser, request
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import time
import datetime
import feedparser
import requests
from sendmail import SendMail


USER_AGENT = "podcastgrb/1.0"
PODCAST_URL = ("https://feeds.soundcloud.com/users/soundcloud"
               ":users:706014772/sounds.rss")


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Datatilsynet().execute()

    return {
        'message': 'OK'
    }


class Datatilsynet():
    """ Datatilsynet backbone. """

    def __init__(self):
        """ Constructor. """
        self.html = ""
        self.error = ""


    def execute(self):
        """ Run the handler. """
        cut_off = datetime.datetime.now() - datetime.timedelta(hours=25)
        xml = self.load_feed()
        if len(xml) == 0:
            title = "hentningsfejl"
            self.deliver_error(title, self.error)
            return

        rss = feedparser.parse(xml)
        if rss.bozo > 0:
            exception = str(rss.bozo_exception)
            if not self.can_cope_with_bozo(exception):
                self.deliver_error("rss parser fejl", exception)
                return

        for item in rss['items']:
            timestamp = time.mktime(item['published_parsed'])
            published_date = datetime.datetime.fromtimestamp(timestamp)
            if published_date < cut_off:
                continue

            link = item['link']
            title = self.title(item['title'])
            body = item['description']
            url = f'<a href="{link}">Lyt til afsnittet</a>'
            html = f'<p>{body}</p><p><br /></p><p>{url}</p>'
            page = self.page(title, html)
            SendMail().deliver(title, page)


    @staticmethod
    def can_cope_with_bozo(bozo_exception):
        """ Can we copy with the bozo exception?
            Args:
                bozo_exception: exception
        """
        bozo_exception = bozo_exception.lower()
        if 'document declared as' in bozo_exception:
            return True

        if 'is not an xml media type' in bozo_exception:
            return True

        if 'no content-type specified' in bozo_exception:
            return True

        return False


    def load_feed(self):
        """ Load Datatilsynets Podcast feed. """
        headers = {'User-Agent': USER_AGENT, 'Connection': 'close'}
        try:
            return requests.get(PODCAST_URL,
                                headers=headers,
                                timeout=30).content.strip()
        except requests.exceptions.Timeout:
            self.error = 'Timeout - waited 30 seconds'
            return ""
        except requests.exceptions.RequestException as req_exc:
            self.error = req_exc
            return ""


    @staticmethod
    def title(suffix):
        """ Get title of mail.
            Arguments:
                suffix - to add to end of title
        """
        now = time.strftime('%Y-%m-%d', time.localtime())
        return f"Datatilsynet {now}: {suffix}"


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


    def deliver_error(self, title, body):
        """ Deliver error email with title and body.
            Arguments:
                title: title suffix
                body: body text
        """
        mail_title = self.title(title)
        body = f'<p style="color: red">{body}</p>'
        page = self.page(mail_title, body)
        SendMail().deliver(mail_title, page)


if __name__ == '__main__':
    Datatilsynet().execute()
