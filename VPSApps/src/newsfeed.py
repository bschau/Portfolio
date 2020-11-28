""" newsfeed """
from datetime import datetime, timedelta
import dbm
import os
import json
import platform
import time
import feedparser
from sendmail import SendMail


feedparser.USER_AGENT = 'newsfeed/4.0'


class Newsfeed():
    """ Newsfeed backbone. """

    def __init__(self):
        """ Init. """
        self.html = ""


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
        file = '_newsfeedrc' if platform.system() == 'Windows' else '.newsfeedrc'

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
        if 'document declared a~s' in bozo_exception:
            return True

        if 'is not an xml media type' in bozo_exception:
            return True

        if 'no content-type specified' in bozo_exception:
            return True

        return False


    def load_feed(self, cache, line_number, feed):
        """ Main method.
            Args:
                self: self
                cache: cache db
                line_number: current line number
                feed: feed
        """
        request_headers = {'Accept-Language': 'da, en'}
        rss = feedparser.parse(feed["RssUrl"],
                               request_headers=request_headers)
        if rss.bozo > 0:
            if not self.can_cope_with_bozo(str(rss.bozo_exception)):
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

                line_number = self.add_news(line_number, item['title'], link)
                cache[link] = cache_time

        if title_emitted:
            self.html = self.html + '</table><p><br /></p>'

        return line_number


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
        return "Newsfeed {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                                   time.localtime()))


    @staticmethod
    def page(title, html):
        """ Get HTML page.
            Arguments:
                title - title
                html - body html
        """
        return """<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{0}</title>
</head>
<body>
{1}
</body>
</html>""".format(title, html)


    def execute(self):
        """ Run the handler. """
        cache = self.get_cache_file()
        self.html = ""

        line_number = 0
        feeds = self.get_feeds()

        for feed in feeds:
            line_number = self.load_feed(cache, line_number, feed)

        if line_number > 0:
            title = self.title()
            page = self.page(title, self.html)
            SendMail('Newsfeed').deliver(title, page)

        self.expire(cache)


if __name__ == '__main__':
    Newsfeed().execute()
