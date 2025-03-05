""" comics """
import ssl
import time
import urllib.request
from sendmail import SendMail


class Comics():
    """ Comics backbone. """

    def __init__(self):
        """ Initialize globals. """
        self.html = ""
        self.goc_date = time.strftime('%Y/%m/%d', time.localtime())


    def execute(self):
        """ Run the handler. """
        title = "Comics {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                                  time.localtime()))
        self.creators('Andy Capp', 'andy-capp')
        self.creators('B.C.', 'bc')
        self.kingdom('Beetle Bailey', 'beetle-bailey')
        self.go_comics('Betty', 'betty')
        self.go_comics('Broom Hilda', 'broomhilda')
        self.go_comics('Calvin and Hobbes', 'calvinandhobbes')
        self.kingdom('Crock', 'crock')

        date = time.strftime('%Y-%m-%d', time.localtime())
        url = 'https://dilbert.com/strip/{}'.format(date)
        self.generic('img-responsive img-comic', 'src="', 'Dilbert', url)

        self.generic('div class="MainComic__ComicImage', 'img src="',
                     'Explosm', 'http://explosm.net')
        self.go_comics('Garfield', 'garfield')
        self.kingdom('Hagar the Horrible', 'hagar-the-horrible')
        self.go_comics('Pearls before Swine', 'pearlsbeforeswine')
        self.kingdom('Sam and Silo', 'sam-and-silo')
        self.kingdom('Zits', 'zits')

        page = self.page(title, self.html)
        SendMail('Comics').deliver(title, page)


    @staticmethod
    def page(title, content):
        """ Get HTML page.
            Arguments:
                title - title
                content - content of page
        """
        return """<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{}</title>
</head>
<body>
{}
</body>
</html>""".format(title, content)


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
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        data = urllib.request.urlopen(request, context=ctx).read()
        return '{}'.format(data)


    @staticmethod
    def entry(url, title):
        """ Get HTML for entry.
            Arguments:
                url - url to comics.
                title - title.
        """
        return """<h1><a href="{0}">{1}</a></h1>
<p><img src="{0}" alt="{1}" width="900" /></p>\r\n
""".format(url, title)


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
            pos = text.find(origin)
            if pos < 0:
                return

            pos = text.find(token, pos)
            start_pos = pos + len(token)
            end_pos = text.find('"', start_pos)
            link = '{}'.format(text[start_pos:end_pos])
            self.html = "{}{}".format(self.html, self.entry(link, title))
        except urllib.error.URLError:
            pass


    def creators(self, title, slug):
        """ Add a Creators comics.
            Args:
                title: title of Creator comics
                slug: short name of Creators comics
        """
        url = "https://www.creators.com/read/{}".format(slug)
        self.generic('class="fancybox"', 'img src="', title, url)


    def kingdom(self, title, slug):
        """ Add a Comics Kingdom comics.
            Args:
                title: title of Comics Kingdom comics
                slug: short name of Comics Kingdom comics
        """
        url = "https://www.comicskingdom.com/{}/".format(slug)
        self.generic('img id="theComicImage"', 'src="', title, url)


    def go_comics(self, title, slug):
        """ Add a Go Comics comics.
            Args:
                title: title of Go Comics comics
                slug: short name of Go Comics comics
        """
        url = 'https://www.gocomics.com/{}/{}'.format(slug, self.goc_date)
        self.generic('item-comic-image', 'src="', title, url)


if __name__ == '__main__':
    Comics().execute()
