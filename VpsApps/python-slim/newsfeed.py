""" newsfeed

Requirements: feedparser, request
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
from datetime import datetime, timedelta
import dbm
import os
import json
import platform
import time
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import feedparser
import requests


USER_AGENT = "blogtrottr/2.0"


class Newsfeed():
    """ Newsfeed backbone. """

    def __init__(self):
        """ Constructor. """
        self.html = ""


    def execute(self):
        """ Run the handler. """
        cache = self.get_cache_file()
        self.html = ""

        line_number = 0
        feeds = self.get_feeds()

        seen = {}
        for feed in feeds:
            line_number = self.load_feed(cache, seen, line_number, feed)

        if line_number > 0:
            title = self.title()
            page = self.page(title, self.html)
            self.deliver(title, page)
            self.persist_seen(cache, seen)

        self.expire(cache)


    @staticmethod
    def get_cache_file():
        """ Get the cache file. """
        file = '_newsfeedT' if platform.system() == 'Windows' else '.newsfeedT'

        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        return dbm.open(full_path, 'c')


    @staticmethod
    def get_feeds():
        """ Get the feeds file. """
        file = '_newsfeeds' if platform.system() == 'Windows' else '.newsfeeds'

        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        with open(full_path, encoding='utf-8') as json_file:
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


    def load_feed(self, cache, seen, line_number, feed):
        """ Main method.
            Args:
                self: self
                cache: cache db
                seen: the empty seen array
                line_number: current line number
                feed: feed
        """
        headers = {'User-Agent': USER_AGENT, 'Connection': 'close'}
        try:
            data = requests.get(feed["RssUrl"],
                                headers=headers,
                                timeout=30).content.strip()
        except requests.exceptions.Timeout:
            self.html = self.html + '<h1><a href="' + feed["WebUrl"]
            self.html = self.html + '">' + feed["Name"] + "</a></h1>"
            self.html = self.html + '<p style="color: red">'
            self.html = self.html + 'Timeout - waited 30 seconds</p>'
            return line_number
        except requests.exceptions.RequestException as req_exc:
            self.html = self.html + '<h1><a href="' + feed["WebUrl"]
            self.html = self.html + '">' + feed["Name"] + "</a></h1>"
            self.html = self.html + '<p style="color: red">'
            self.html = self.html + str(req_exc) + '</p>'
            return line_number

        rss = feedparser.parse(data)
        if rss.bozo > 0:
            exception = str(rss.bozo_exception)
            if not self.can_cope_with_bozo(exception):
                self.html = self.html + '<h1><a href="' + feed["WebUrl"]
                self.html = self.html + '">' + feed["Name"] + "</a></h1>"
                self.html = self.html + '<p style="color: red">'
                self.html = self.html + exception + '</p>'
                return line_number

        cache_time = time.strftime('%Y/%m/%d %H:%M:%S', time.localtime())
        title_emitted = False
        for item in rss['items']:
            link = item['link']
            if cache.get(link, '') == '':
                if not title_emitted:
                    self.html = self.html + '<h1><a href="' + feed["WebUrl"]
                    self.html = self.html + '">' + feed["Name"] + "</a></h1>"
                    self.html = self.html + '<table style="width: 100%">'
                    title_emitted = True

                line_number = self.add_news(line_number,
                                            item['title'],
                                            link)
                seen[link] = cache_time

        if title_emitted:
            self.html = self.html + '</table><p><br /></p>'

        return line_number


    @staticmethod
    def persist_seen(cache, seen):
        """ Add seen link to cache - happens after attempt to send email.
            Arguments:
                cache: cache db
                seen: array with seen links
        """
        for link in seen:
            cache[link] = seen[link]


    @staticmethod
    def expire(cache):
        """ Remove old entries and close database.
            Arguments:
                cache: cache db
        """
        keys = cache.keys()
        cutoff = datetime.now() - timedelta(365)
        for key in keys:
            decoded = cache[key].decode('utf-8')
            key_date = datetime.strptime(decoded, '%Y/%m/%d %H:%M:%S')
            if key_date >= cutoff:
                continue

            del cache[key]

        cache.close()


    def add_news(self, line_number, title, url):
        """ Add a newsfeed line to mail.
            Args:
                self: self
                line_number: current line number
                title: title of news
                url: deep link to article
        """
        self.html = self.html + '<tr><td style="border: 1px solid #adadad;'
        self.html = self.html + 'background-color: #f3f1ec; color: #666;'
        self.html = self.html + 'padding:0.5em;font-size:120%"><a href="'
        self.html = self.html + url
        self.html = self.html + '">' + title + '</a>'
        self.html = self.html + '</td></tr>\r\n'
        line_number = line_number + 1
        return line_number


    @staticmethod
    def title():
        """ Get title of mail. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        return f"Newsfeed {now}"


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


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian@schau.dk"]
        message = MIMEMultipart('alternative')
        message['Subject'] = title
        message['From'] = "bs@leah.schau.dk"
        message['To'] = ",".join(recipients)
        plain_text = MIMEText(text, 'plain')
        html_text = MIMEText(body, 'html')
        message.attach(plain_text)
        message.attach(html_text)

        mail = smtplib.SMTP('localhost')
        mail.sendmail(message['From'], recipients, message.as_string())
        mail.quit()


if __name__ == '__main__':
    Newsfeed().execute()
