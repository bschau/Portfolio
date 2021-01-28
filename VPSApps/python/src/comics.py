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

        comics = self.fetch_explosm()
        html = "{0}{1}".format(html, comics)

        url = 'https://www.comicskingdom.com/hagar-the-horrible/'
        comics = self.generic(ck_origin, ck_token, 'Hagar', url)
        html = "{0}{1}".format(html, comics)

        comics = self.fetch_ingemann()
        html = "{0}{1}".format(html, comics)

        comics = self.fetch_monkeyuser()
        html = "{0}{1}".format(html, comics)

        comics = self.fetch_xkcd()
        html = "{0}{1}".format(html, comics)

        url = 'https://www.comicskingdom.com/zits'
        comics = self.generic(ck_origin, ck_token, 'Zits', url)
        html = "{0}{1}".format(html, comics)

        SendMail('Comics').deliver(title, page.format(html))


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


    def fetch_monkeyuser(self):
        """ Add the Monkey User comics. """
        try:
            url = 'https://www.monkeyuser.com'
            text = self.fetch(url)
            pos = text.find('div class="content">')
            pos = text.find('src', pos)
            start_pos = pos + 5
            end_pos = text.find('"', start_pos)
            link = "{}".format(text[start_pos:end_pos])
            html = self.header('Monkey User', url)
            html = self.comics(html, link, 'Monkey User', 900)
            return self.line_break(html)
        except urllib.error.URLError:
            return ""


    def fetch_explosm(self):
        """ Add the Explosm comics. """
        try:
            url = "http://explosm.net"
            text = self.fetch(url)
            html = self.header('Explosm', url)
            pos = text.find("div id=\"comic-wrap\"")
            if pos >= 0:
                pos = text.find("flex-video", pos)
                if pos >= 0:
                    body = """
<p>Today is a video. Click <a href=\"{0}\">here</a> to see it :-)</p>
""".format(url)
                    html = "{0}{1}".format(html, body)
                    return self.line_break(html)

            pos = text.find("img id=\"main-comic\" src")
            start_pos = pos + 25
            end_pos = text.find('"', start_pos)
            link = '{}'.format(text[start_pos:end_pos])
            if link.startswith('//'):
                link = 'http:{}'.format(link)

            html = self.comics(html, link, 'Explosm', 900)
            return self.line_break(html)
        except urllib.error.URLError:
            return ""


    def fetch_xkcd(self):
        """ Add the XKCD comics. """
        try:
            url = 'http://xkcd.com'
            text = self.fetch(url)
            search = '<img src="'
            pos = text.find('<div id="comic">')
            pos = text.find(search, pos)
            start_pos = pos + len(search)
            end_pos = text.find('"', start_pos)

            search = 'title="'
            pos = text.find(search, end_pos)
            title_start = pos + len(search)
            title_end = text.find('"', title_start)
            link = "http:{}".format(text[start_pos:end_pos])
            alt = '{}'.format(text[title_start:title_end])
            html = self.header('XKCD', url)
            html = self.comics(html, link, 'XKCD')
            html = self.line_break(html, alt)
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

    def fetch_ingemann(self):
        """ Fetch the Ingemann comic from Ekstrabladet. """
        try:
            ebroot = "https://ekstrabladet.dk"
            text = self.fetch(ebroot + '/ingemann/')
            search = "flex-item mar-l--b"
            pos = text.find(search)
            if pos < 0:
                return ""
            search = "a href"
            start_pos = text.find(search, pos) + len(search) + 2
            end_pos = text.find('"', start_pos)
            url = ebroot + text[start_pos:end_pos]
            text = self.fetch(url)
            search = "image-container"
            pos = text.find(search)
            if pos < 0:
                return ""
            search = "src"
            start_pos = text.find(search, pos) + len(search) + 2
            end_pos = text.find('"', start_pos)
            title = "Ingemann"
            link = ebroot + text[start_pos:end_pos]
            html = self.header(title, url)
            html = self.comics(html, link, title, 900)
            return self.line_break(html)
        except urllib.error.URLError:
            return ""


if __name__ == '__main__':
    Comics().execute()
