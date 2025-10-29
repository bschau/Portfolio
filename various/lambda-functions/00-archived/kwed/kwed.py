""" kwed

Requirements:   feedparser>=5.1.3

Install with:
    pip3 install 'feedparser>=5.1.3'
    pip3 install requests
"""
import time
import datetime
from urllib.parse import quote
from http.client import HTTPSConnection
import feedparser
from sendmail import SendMail


PREFIX = 'New C64 remix released: '
PREFIX_LEN = len(PREFIX)
HTML_ESCAPE = {
    "&": "&amp;",
    '"': "&quot;",
    "'": "&apos;",
    ">": "&gt;",
    "<": "&lt;"
}

def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Kwed().execute()

    return {
        'message': 'OK'
    }


class Kwed():
    """ Kwed backbone. """


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


    def execute(self):
        """ Run the handler. """
        cut_off = datetime.datetime.now() - datetime.timedelta(hours=25)
        feedparser.USER_AGENT = 'kwed-lambda/4.0'
        request_headers = {'Accept-Language': 'da, en'}
        rss = feedparser.parse('https://remix.kwed.org/rss.xml',
                               request_headers=request_headers)
        if rss.bozo > 0:
            if not self.can_cope_with_bozo(str(rss.bozo_exception)):
                return

        html = ''
        for item in rss['items']:
            timestamp = time.mktime(item['published_parsed'])
            published_date = datetime.datetime.fromtimestamp(timestamp)
            if published_date < cut_off:
                continue

            title = item['title']
            if title.startswith(PREFIX):
                title = title[PREFIX_LEN:]

            tid = int(item['link'].split('/')[-1])
            url = self.get_download_url(tid)

            track_title = "".join(HTML_ESCAPE.get(c, c) for c in title)
            html = f'{html}<li><a href="{url}">{track_title}</a></li>\n'

        if len(html) > 0:
            title = self.title()
            page = self.head(title)
            page = f'{page}<html><body><ul>{html}</ul></body></html>'
            SendMail().deliver(title, page)


    @staticmethod
    def get_download_url(file_id):
        """ Get download link for link.
            Arguments:
                file_id: remix.kwed.org id
        """
        connection = HTTPSConnection('remix.kwed.org')
        connection.request('HEAD', '/download.php/' + str(file_id))
        response = connection.getresponse()
        location = response.getheader('Location')
        if location is None:
            return None

        return 'https://remix.kwed.org' + quote(location)


    @staticmethod
    def title():
        """ Get title of mail. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        return f"KWED {now}"


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


if __name__ == '__main__':
    Kwed().execute()
