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

    def execute(self):
        """ Run the handler. """
        cut_off = datetime.datetime.now() - datetime.timedelta(hours=25)
        feedparser.USER_AGENT = 'kwed-lambda/2.0'
        rss = feedparser.parse('https://remix.kwed.org/rss.xml')
        if rss.bozo > 0:
            return

        prefix = 'New C64 remix released: '
        prefix_len = len(prefix)
        html = ''
        html_escape = {
            "&": "&amp;", '"': "&quot;", "'": "&apos;", ">": "&gt;", "<": "&lt;"
        }
        for item in rss['items']:
            timestamp = time.mktime(item['published_parsed'])
            published_date = datetime.datetime.fromtimestamp(timestamp)
            if published_date < cut_off:
                continue

            title = item['title']
            if title.startswith(prefix):
                title = title[prefix_len:]

            tid = int(item['link'].split('=')[-1])
            url = self.get_download_url(tid)

            track_title = "".join(html_escape.get(c, c) for c in title)
            html = html + '<a href="' + url + '">'
            html = html + track_title + '</a><br />'

        if len(html) == 0:
            return

        title = self.title()
        page = self.page(title)
        SendMail('KWED').deliver(title, page.format(html))


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
        return "KWED {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                               time.localtime()))


    @staticmethod
    def page(title):
        """ Get HTML page.
            Arguments:
                title - title.
        """
        return """<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{0}</title>
</head>
<body>
<p>{{0}}</p>
</body>
</html>""".format(title)


if __name__ == '__main__':
    Kwed().execute()
