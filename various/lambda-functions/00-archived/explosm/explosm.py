""" explosm """
import ssl
import time
import urllib.request
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Explosm().execute()

    return {
        'message': 'OK'
    }


class Explosm():
    """ Explosm backbone. """

    def __init__(self):
        """ Initialize globals. """
        self.html = ""
        self.goc_date = time.strftime('%Y/%m/%d', time.localtime())


    def execute(self):
        """ Run the handler. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        title = f'Explosm {now}'
        url = "http://explosm.net"

        self.html = self.html + f'<h1><a href="{url}">Explosm</a></h1>'
        try:
            text = self.fetch(url)
            pos = text.find('<div class="MainComic__ComicImage')
            if pos < 0:
                return

            token = 'img src="'
            pos = text.find(token, pos)
            start_pos = pos + len(token)
            end_pos = text.find('"', start_pos)
            link = text[start_pos:end_pos]
            self.html = self.html + f'<p><img src="{link}" alt="Explosm" '
            self.html = self.html + 'width="800" /></p>\r\n'
        except urllib.error.URLError:
            self.html = self.html + '<p>Comic could not be fetched.</p>'

        page = self.page(title, self.html)
        SendMail().deliver(title, page)


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
            return f'{data}'


if __name__ == '__main__':
    Explosm().execute()
