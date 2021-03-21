""" comics """
import time
import urllib
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Comics().execute()

    return {
        'message': 'OK'
    }


class Comics():
    """ Comics backbone. """

    def execute(self):
        """ Run the handler. """
        title = self.title()
        page = self.page(title)
        c_origin = 'class="fancybox"'
        c_token = 'img src'
        ck_origin = False
        ck_token = 'data-comic-image-url'

        url = 'https://www.creators.com/read/andy-capp'
        html = self.generic(c_origin, c_token, 'Andy Capp', url)

        url = 'https://www.creators.com/read/bc'
        comics = self.generic(c_origin, c_token, 'B.C.', url)
        html = "{0}{1}".format(html, comics)

        comics = self.fetch_dilbert()
        html = "{0}{1}".format(html, comics)

        url = 'https://www.comicskingdom.com/hagar-the-horrible/'
        comics = self.generic(ck_origin, ck_token, 'Hagar', url)
        html = "{0}{1}".format(html, comics)

        url = 'https://www.comicskingdom.com/zits'
        comics = self.generic(ck_origin, ck_token, 'Zits', url)
        html = "{0}{1}".format(html, comics)

        SendMail().deliver(title, page.format(html))


    @staticmethod
    def title():
        """ Get title of mail. """
        return "Comics {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
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
{{0}}
</body>
</html>""".format(title)


    @staticmethod
    def header(header, link):
        """ Add header.
            Args:
                header: header text
                link: link to comics site
        """
        return '<h1><a href="{0}">{1}</a></h1>'.format(link, header)


    @staticmethod
    def comics(html, image_link, alt_text, width=0):
        """ Add comics link to html.
            Args:
                html: existing html
                image_link: image data to inline
                alt_text: text to the 'alt' attribute
                width: optional width
        """
        html = '{0}<p><img src="{1}" alt="{2}"'.format(html, image_link,
                                                       alt_text)
        if width > 0:
            html = '{0} width="{1}"'.format(html, str(width))
        return "{0} /></p>".format(html)


    @staticmethod
    def line_break(html, break_text='<br />'):
        """ Add line break to HTML.
            Args:
                html: existing html
                break_text: the text between p / p
        """
        return '{0}<p>{1}</p>\r\n'.format(html, break_text)


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
            }
        )
        data = urllib.request.urlopen(request).read()
        return '{}'.format(data)


    def fetch_dilbert(self):
        """ Add the Dilbert comics.
            Arguments:
                vps: VpsUtils
        """
        try:
            url = 'https://dilbert.com'
            date = time.strftime('%Y-%m-%d', time.localtime())
            remote = '{}/strip/{}'.format(url, date)
            text = self.fetch(remote)
            pos = text.find('img-responsive img-comic')
            pos = text.find('src', pos)
            start_pos = pos + 5
            end_pos = text.find('"', start_pos)
            link = text[start_pos:end_pos]
            html = self.header('Dilbert', url)
            html = self.comics(html, link, 'Dilbert')
            return self.line_break(html)
        except urllib.error.URLError:
            return ""


    def generic(self, origin, token, title, url):
        """ Add the generic comics.
            Args:
                origin: first tag to search for
                token: next tag to search for
                title: title of King Features comics
                url: url to King Features comics
        """
        try:
            text = self.fetch(url)
            if origin is False:
                pos = 0
            else:
                pos = text.find(origin)

            pos = text.find(token, pos)
            start_pos = pos + len(token) + 2
            end_pos = text.find('"', start_pos)
            link = '{}'.format(text[start_pos:end_pos])
            html = self.header(title, url)
            html = self.comics(html, link, title, 900)
            return self.line_break(html)
        except urllib.error.URLError:
            return ""


if __name__ == '__main__':
    Comics().execute()
