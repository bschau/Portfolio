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
import os
import time
import feedparser


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
        html = ''
        for item in rss['items']:
            tid = int(item['link'].split('/')[-1])
            if tid <= self.base_counter:
                continue

            title = item['title']
            if title.startswith(PREFIX):
                title = title[PREFIX_LEN:]

            url = self.get_download_url(tid)

            track_title = "".join(HTML_ESCAPE.get(c, c) for c in title)
            html = html + '<li><a href="' + url + '">'
            html = html + track_title + '</a></li>\n'
            if tid > counter:
                counter = tid

        if counter > self.base_counter:
            with open(self.counter_file, "w+", encoding='utf8') as kwedfile:
                kwedfile.write(str(counter))

        if len(html) > 0:
            title = self.title()
            page = self.page(title, html)
            self.deliver(title, page)


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
    def page(title, items):
        """ Get HTML page.
            Arguments:
                title - title.
                items - items.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>
<body>
<p><ul>{items}</ul></p>
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
        message['From'] = "bs@phoebe.schau.dk"
        message['To'] = ",".join(recipients)
        plain_text = MIMEText(text, 'plain')
        html_text = MIMEText(body, 'html')
        message.attach(plain_text)
        message.attach(html_text)

        mail = smtplib.SMTP('localhost')
        mail.sendmail(message['From'], recipients, message.as_string())
        mail.quit()


if __name__ == '__main__':
    Kwed().execute()
