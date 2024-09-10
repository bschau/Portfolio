""" comics

Requirements: requests
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import ssl
import time
import urllib.request
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


class Comics():
    """ Comics backbone. """

    def __init__(self):
        """ Initialize globals. """
        self.html = ""
        self.goc_date = time.strftime('%Y/%m/%d', time.localtime())


    def execute(self):
        """ Run the handler. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f"Comics {now}"
        self.creators('Andy Capp', 'andy-capp')
        self.creators('B.C.', 'bc')
        self.go_comics('Betty', 'betty')
        self.go_comics('Broom Hilda', 'broomhilda')
        self.go_comics('Calvin and Hobbes', 'calvinandhobbes')

        self.generic('div class="MainComic__ComicImage', 'img src="',
                     'Explosm', 'http://explosm.net')
        self.go_comics('Garfield', 'garfield')
        self.go_comics('Pearls before Swine', 'pearlsbeforeswine')

        page = self.page(title, self.html)
        self.deliver(title, page)


    @staticmethod
    def page(title, content):
        """ Get HTML page.
            Arguments:
                title - title
                content - content of page
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>
<body>
{content}
<p><br /></p>
<p><br /></p>
<p>Delivered by the Comics service!</p>
<p><br /></p>
</body>
</html>"""


    @staticmethod
    def fetch(url):
        """ Fetch content of url.
            Arguments:
                url - url to fetch
        """
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            },
        )
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        with urllib.request.urlopen(request, context=ctx) as web:
            data = web.read()
            return f"{data}"


    @staticmethod
    def entry(url, title):
        """ Get HTML for entry.
            Arguments:
                url - url to comics.
                title - title.
        """
        return f"""<h1><a href="{url}">{title}</a></h1>
<p><img src="{url}" alt="{title}" width="800" /></p>\r\n
"""


    @staticmethod
    def error(url, title):
        """ Get HTML for error-entry.
            Arguments:
                url - url to comics.
                title - title.
        """
        return f"""<h1>{title}</h1>
<p>Comic could not be fetched: <a href="{url}">{url}</a></p>\r\n
"""


    def generic(self, origin, token, title, url):
        """ Add the generic comics.
            Args:
                origin: first tag to search for
                token: next tag to search for
                title: title of comics
                url: url to comics
        """
        try:
            text = self.fetch(url)
            if origin is False:
                pos = 0
            else:
                pos = text.find(origin)
                if pos < 0:
                    return

            pos = text.find(token, pos)
            start_pos = pos + len(token)
            end_pos = text.find('"', start_pos)
            entry = self.entry(text[start_pos:end_pos], title)
            self.html = f"{self.html}{entry}"
        except urllib.error.URLError:
            error = self.error(url, title)
            self.html = f"{self.html}{error}"


    def creators(self, title, slug):
        """ Add a Creators comics.
            Args:
                title: title of Creator comics
                slug: short name of Creators comics
        """
        url = f"https://www.creators.com/read/{slug}"
        self.generic('class="fancybox"', 'img src="', title, url)


    def go_comics(self, title, slug):
        """ Add a Go Comics comics.
            Args:
                title: title of Go Comics comics
                slug: short name of Go Comics comics
        """
        url = f"https://www.gocomics.com/{slug}/{self.goc_date}"
        self.generic('item-comic-image', 'src="', title, url)


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
    Comics().execute()
