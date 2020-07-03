""" comics """
import time
import urllib
from vpsutils import VpsUtils


def header(text, link):
    """ Add header.
        Args:
            text: header text
            link: link to comics site
    """
    return '<h1><a href="{0}">{1}</a></h1>'.format(link, text)


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


def line_break(html, break_text='<br />'):
    """ Add line break to HTML.
        Args:
            html: existing html
            break_text: the text between p / p
    """
    return '{0}<p>{1}</p>\r\n'.format(html, break_text)


def fetch(url):
    """ Fetch content of url.
        Args:
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


def fetch_dilbert(vps):
    """ Add the Dilbert comics.
        Args:
            vps: VpsUtils
    """
    try:
        url = 'http://dilbert.com'
        date = time.strftime('%Y-%m-%d', time.localtime())
        remote = '{}/strip/{}'.format(url, date)
        text = fetch(remote)
        pos = text.find('img-responsive img-comic')
        pos = text.find('src', pos)
        start_pos = pos + 5
        end_pos = text.find('"', start_pos)
        link = "https:{}".format(text[start_pos:end_pos])
        html = header('Dilbert', url)
        html = comics(html, link, 'Dilbert')
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def fetch_monkeyuser(vps):
    """ Add the Monkey User comics.
        Args:
            vps: VpsUtils
    """
    try:
        url = 'https://www.monkeyuser.com'
        text = fetch(url)
        pos = text.find('div class="content">')
        pos = text.find('src', pos)
        start_pos = pos + 5
        end_pos = text.find('"', start_pos)
        link = "{}".format(text[start_pos:end_pos])
        html = header('Monkey User', url)
        html = comics(html, link, 'Monkey User', 900)
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def fetch_explosm(vps):
    """ Add the Explosm comics.
        Args:
            vps: VpsUtils
    """
    try:
        url = "http://explosm.net"
        text = fetch(url)
        html = header('Explosm', url)
        pos = text.find("div id=\"comic-wrap\"")
        if pos >= 0:
            pos = text.find("flex-video", pos)
            if pos >= 0:
                body = """
<p>Today is a video. Click <a href=\"{0}\">here</a> to see it :-)</p>
""".format(url)
                html = "{0}{1}".format(html, body)
                html = line_break(html)
                vps.html_add(html)
                return

        pos = text.find("img id=\"main-comic\" src")
        start_pos = pos + 25
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        if link.startswith('//'):
            link = 'http:{}'.format(link)

        html = comics(html, link, 'Explosm', 900)
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def fetch_xkcd(vps):
    """ Add the XKCD comics.
        Arguments:
            vps: VpsUtils
    """
    try:
        url = 'http://xkcd.com'
        text = fetch(url)
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
        html = header('XKCD', url)
        html = comics(html, link, 'XKCD')
        html = line_break(html, alt)
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def generic(vps, origin, token, title, url):
    """ Add the generic comics.
        Args:
            vps: VpsUtils
            origin: first tag to search for
            token: next tag to search for
            title: title of King Features comics
            url: url to King Features comics
    """
    try:
        text = fetch(url)
        if origin is False:
            pos = 0
        else:
            pos = text.find(origin)

        pos = text.find(token, pos)
        start_pos = pos + len(token) + 2
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        html = header(title, url)
        html = comics(html, link, title, 900)
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def fetch_ingemann(vps):
    """ Fetch the Ingemann comic from Ekstrabladet.
        Arguments:
            vps: VpsUtils
    """
    try:
        ebroot = "https://ekstrabladet.dk"
        text = fetch(ebroot + '/ingemann/')
        search = "flex-item mar-l--b"
        pos = text.find(search)
        if pos < 0:
            return

        search = "a href"
        start_pos = text.find(search, pos) + len(search) + 2
        end_pos = text.find('"', start_pos)
        url = ebroot + text[start_pos:end_pos]
        text = fetch(url)
        search = "image-container"
        pos = text.find(search)
        if pos < 0:
            return

        search = "src"
        start_pos = text.find(search, pos) + len(search) + 2
        end_pos = text.find('"', start_pos)
        title = "Ingemann"
        link = ebroot + text[start_pos:end_pos]
        html = header(title, url)
        html = comics(html, link, title, 900)
        html = line_break(html)
        vps.html_add(html)
    except urllib.error.URLError:
        pass


def main():
    """ Main loop. """
    vps = VpsUtils('Comics')
    c_origin = 'class="fancybox"'
    c_token = 'img src'
    ck_origin = False
    ck_token = 'data-comic-image-url'

    url = 'https://www.creators.com/read/andy-capp'
    generic(vps, c_origin, c_token, 'Andy Capp', url)

    url = 'https://www.creators.com/read/bc'
    generic(vps, c_origin, c_token, 'B.C.', url)

    fetch_dilbert(vps)

    fetch_explosm(vps)

    url = 'https://www.comicskingdom.com/hagar-the-horrible/'
    generic(vps, ck_origin, ck_token, 'Hagar', url)

    fetch_ingemann(vps)

    fetch_monkeyuser(vps)

    fetch_xkcd(vps)

    url = 'https://www.comicskingdom.com/zits'
    generic(vps, ck_origin, ck_token, 'Zits', url)

    vps.deliver()


if __name__ == "__main__":
    main()
