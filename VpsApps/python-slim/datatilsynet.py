""" datatilsynet

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
import platform
import time
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import feedparser
import requests


USER_AGENT = "podcastgrb/1.0"
PODCAST_URL = ("https://feeds.soundcloud.com/users/soundcloud"
               ":users:706014772/sounds.rss")


class Datatilsynet():
    """ Datatilsynet backbone. """

    def __init__(self):
        """ Constructor. """
        self.html = ""
        self.error = ""


    def execute(self):
        """ Run the handler. """
        cache = self.get_cache_file()
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

        cache_time = time.strftime('%Y/%m/%d %H:%M:%S', time.localtime())
        seen = {}
        for item in rss['items']:
            link = item['link']
            if cache.get(link, '') == '':
                title = self.title(item['title'])
                body = item['description']
                url = f'<a href="{link}">Lyt til afsnittet</a>'
                html = f'<p>{body}</p><p><br /></p><p>{url}</p>'
                page = self.page(title, html)
                self.deliver(title, page)
                seen[link] = cache_time

        if len(seen) > 0:
            self.persist_seen(cache, seen)

        self.expire(cache)


    @staticmethod
    def get_cache_file():
        """ Get the cache file. """
        file = '_dtgrbT' if platform.system() == 'Windows' else '.dtgrbT'

        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        return dbm.open(full_path, 'c')


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
        self.deliver(mail_title, page)


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian.schau@twoday.com"]
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
    Datatilsynet().execute()
