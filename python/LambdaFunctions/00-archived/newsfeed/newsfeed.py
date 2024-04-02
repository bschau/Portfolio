""" newsfeed """
import datetime
import json
import os
import time
import feedparser
import requests
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Newsfeed().execute()

    return {
        'message': 'OK'
    }


class Newsfeed():
    """ Newsfeed backbone. """

    def __init__(self):
        """ Constructor. """
        self.html = ""
        feedparser.USER_AGENT = 'newsfeed/5.0'


    def execute(self):
        """ Run the handler. """
        cut_off_hours = int(os.environ["CUT_OFF_HOURS"])
        adjust = datetime.timedelta(hours=cut_off_hours, minutes=5)
        cut_off = datetime.datetime.now() - adjust
        self.html = ""

        line_number = 0
        feeds = self.get_feeds()

        for feed in feeds:
            line_number = self.load_feed(line_number, feed, cut_off)

        if line_number > 0:
            title = self.title()
            page = self.page(title)
            page = f"{page}<body>{self.html}</body></html>"
            SendMail().deliver(title, page)


    @staticmethod
    def get_feeds():
        """ Get the feeds file. """
        with open("feeds.json", encoding='utf-8') as json_file:
            return json.load(json_file)


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


    def load_feed(self, line_number, feed, cut_off):
        """ Main method.
            Args:
                self: self
                seen: the empty seen array
                line_number: current line number
                feed: feed
                cut_off: cut off datetime
        """
        request_headers = {'Accept-Language': 'da, en'}
        try:
            rss = feedparser.parse(feed["RssUrl"],
                                   request_headers=request_headers)
        except requests.exceptions.RequestException as exception:
            self.html = self.html + '<p style="color: red">'
            self.html = self.html + exception + '</p>'
            return line_number

        if rss.bozo > 0:
            if not self.can_cope_with_bozo(str(rss.bozo_exception)):
                return line_number

        title_emitted = False
        for item in rss['items']:
            timestamp = time.mktime(item['published_parsed'])
            published_date = datetime.datetime.fromtimestamp(timestamp)
            if published_date < cut_off:
                continue

            link = item['link']
            if not title_emitted:
                self.html = self.html + '<h1><a href="' + feed["WebUrl"]
                self.html = self.html + '">' + feed["Name"] + "</a></h1>"
                self.html = self.html + '<table style="width: 100%">'
                title_emitted = True

            line_number = self.add_news(line_number, item['title'], link)

        if title_emitted:
            self.html = self.html + '</table><p><br /></p>'

        return line_number


    def add_news(self, line_number, title, url):
        """ Add a newsfeed line to mail.
            Args:
                self: self
                line_number: current line number
                title: title of news
                url: deep link to article
        """
        color = '#efe' if line_number % 2 == 1 else '#fff'
        self.html = self.html + '<tr><td style="background-color: '
        self.html = self.html + color
        self.html = self.html + ';padding:0.5em;font-size:120%"><a href="'
        self.html = self.html + url
        self.html = self.html + '">' + title + '</a></td></tr>\r\n'
        line_number = line_number + 1
        return line_number


    @staticmethod
    def title():
        """ Get title of mail. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        return f"Newsfeed {now}"


    @staticmethod
    def page(title):
        """ Get HTML page.
            Arguments:
                title - title
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>"""


if __name__ == '__main__':
    Newsfeed().execute()
