""" kwed

Requirements:   feedparser, requests
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
from urllib.parse import quote
from http.client import HTTPSConnection
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
import email.encoders
import os
import feedparser
import requests


PREFIX = 'New C64 remix released: '
PREFIX_LEN = len(PREFIX)
HTML_ESCAPE = {
    "&": "&amp;", '"': "&quot;", "'": "&apos;", ">": "&gt;", "<": "&lt;"
}

class Kwed():
    """ Kwed backbone. """

    def __init__(self):
        """ Prepare class for action. """
        home = os.path.expanduser("~")
        self.counter_file = f"{home}/.kwed"
        with open(self.counter_file, "r", encoding='utf8') as counter:
            self.base_counter = int(counter.readline().strip())


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


    def get_rss(self):
        """ Get the remote RSS. """
        feedparser.USER_AGENT = 'kwed-vps/1.0'
        request_headers = {'Accept-Language': 'da, en'}
        rss = feedparser.parse('https://remix.kwed.org/rss.xml',
                               request_headers=request_headers)
        if rss.bozo > 0:
            if not self.can_cope_with_bozo(str(rss.bozo_exception)):
                return None

        return rss


    def execute(self):
        """ Run the handler. """
        rss = self.get_rss()
        if rss is None:
            return

        counter = self.base_counter
        for item in rss['items']:
            tid = int(item['link'].split('/')[-1])
            if tid <= self.base_counter:
                continue

            title = item['title']
            if title.startswith(PREFIX):
                title = title[PREFIX_LEN:]

            url = self.get_download_url(tid)
            mp3 = requests.get(url).content
            if mp3 is None:
                continue

            filename = title.translate({ord(c): None for c in "<>&:\\/"})
            track_title = "".join(HTML_ESCAPE.get(c, c) for c in title)
            page = self.page(track_title)

            html = '<p><a href="' + url + '">'
            html = html + track_title + '</a></p>\n'
            self.deliver(title, page.format(html), filename, mp3)

            if tid > counter:
                counter = tid

        if counter > self.base_counter:
            with open(self.counter_file, "w+", encoding='utf8') as counter_file:
                counter_file.write(str(counter))


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
    def page(title):
        """ Get HTML page.
            Arguments:
                title - title.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>KWED: {0}</title>
</head>
<body>
<p><ul>{title}</ul></p>
</body>
</html>"""


    @staticmethod
    def deliver(title, body, filename, mp3):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
                filename: mp3 filename
                mp3: mp3 content
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian.schau@twoday.com"]
        message = MIMEMultipart('alternative')
        message['Subject'] = f"KWED: {title}"
        message['From'] = "bs@leah.schau.dk"
        message['To'] = ",".join(recipients)
        plain_text = MIMEText(text, 'plain')
        html_text = MIMEText(body, 'html')
        message.attach(plain_text)
        message.attach(html_text)
        mp3_section = MIMEBase('audio', 'mpeg')
        mp3_section.set_payload(mp3)
        email.encoders.encode_base64(mp3_section)
        mp3_section.add_header('Content-Disposition',
                               f'attachment; filename="{filename}.mp3"')
        message.attach(mp3_section)

        mail = smtplib.SMTP('localhost')
        mail.sendmail(message['From'], recipients, message.as_string())
        mail.quit()


if __name__ == '__main__':
    Kwed().execute()
