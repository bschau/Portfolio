""" comics """
import time
import urllib
from vpsutils import VpsUtils


def add_html_header(vps, header, link):
    """ Add header.
        Args:
            vps: VpsUtils
            header: header text
            link: link to comics site
    """
    vps.html_add('<h1><a href="' + link + '">' + header + '</a></h1>')


def add_comics(vps, image_link, alt_text, width=0):
    """ Add comics link to html.
        Args:
            vps: VpsUtils
            image_link: image data to inline
            alt_text: text to the 'alt' attribute
            width: optional width
    """
    vps.html_add('<p><img src="' + image_link + '" alt="')
    vps.html_add(alt_text + '"')
    if width > 0:
        vps.html_add(' width="' + str(width) + '"')
    vps.html_add(' /></p>')


def add_line_break(vps, break_text='<br />'):
    """ Add line break to HTML.
        Args:
            vps: VpsUtils
            break_text: the text between p / p
    """
    vps.html_add('<p>{}</p>\r\n'.format(break_text))


def fetch_dilbert(vps):
    """ Add the Dilbert comics.
        Arguments:
            vps: VpsUtils
    """
    try:
        url = 'http://dilbert.com'
        date = time.strftime('%Y-%m-%d', time.localtime())
        remote = '{}/strip/{}'.format(url, date)
        request = urllib.request.Request(
            remote, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
        pos = text.find('img-responsive img-comic')
        pos = text.find('src', pos)
        start_pos = pos + 5
        end_pos = text.find('"', start_pos)
        link = "https:{}".format(text[start_pos:end_pos])
        add_html_header(vps, 'Dilbert', url)
        add_comics(vps, link, 'Dilbert')
        add_line_break(vps)
    except urllib.error.URLError:
        return


def fetch_monkeyuser(vps):
    """ Add the Monkey User comics.
        Arguments:
            vps: VpsUtils
    """
    try:
        url = 'https://www.monkeyuser.com'
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
        pos = text.find('div class="content">')
        pos = text.find('src', pos)
        start_pos = pos + 5
        end_pos = text.find('"', start_pos)
        link = "{}".format(text[start_pos:end_pos])
        add_html_header(vps, 'Monkey User', url)
        vps.html_add('<p><img src="' + link + '" alt="Monkey User" ')
        vps.html_add('width="900" /></p>')
        add_line_break(vps)
    except urllib.error.URLError:
        return


def fetch_explosm(vps):
    """ Add the Explosm comics.
        Arguments:
            vps: VpsUtils
    """
    try:
        url = "http://explosm.net"
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
        add_html_header(vps, "Explosm", url)
        pos = text.find("div id=\"comic-wrap\"")
        if pos >= 0:
            pos = text.find("flex-video", pos)
            if pos >= 0:
                vps.html_add("<p>Today is a video. Click <a href=\"" + url)
                vps.html_add("\">here</a> to see it :-)</p>")
                add_line_break(vps)
                return

        pos = text.find("img id=\"main-comic\" src")
        start_pos = pos + 25
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        if link.startswith('//'):
            link = 'http:{}'.format(link)

        vps.html_add('<p><img src="' + link + '" alt="Explosm" ')
        vps.html_add('width="900" /></p>')
        add_line_break(vps)
    except urllib.error.URLError:
        return


def fetch_xkcd(vps):
    """ Add the XKCD comics.
        Arguments:
            vps: VpsUtils
    """
    try:
        url = 'http://xkcd.com'
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
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
        add_html_header(vps, 'XKCD', url)
        add_comics(vps, link, 'XKCD')
        add_line_break(vps, alt)
        add_line_break(vps)
    except urllib.error.URLError:
        return


def fetch_comicskingdom(vps, title, url):
    """ Add the Comics Kingdom comic.
        Args:
            vps: VpsUtils
            title: title of King Features comics
            url: url to King Features comics
    """
    try:
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
        pos = text.find('data-comic-image-url')
        start_pos = pos + 22
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        add_html_header(vps, title, url)
        add_comics(vps, link, title, 900)
        add_line_break(vps)
    except urllib.error.URLError:
        return


def fetch_generic(vps, origin, title, url):
    """ Add the generic comics.
        Args:
            vps: VpsUtils
            origin: first tag to search for
            title: title of King Features comics
            url: url to King Features comics
    """
    try:
        request = urllib.request.Request(
            url, data=None,
            headers={
                'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                               'rv:55.0) Gecko/20100101 Firefox/55.0')
            }
        )
        response = urllib.request.urlopen(request)
        data = response.read()
        text = '{}'.format(data)
        pos = text.find(origin)
        pos = text.find("img src", pos)
        start_pos = pos + 9
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        add_html_header(vps, title, url)
        add_comics(vps, link, title)
        add_line_break(vps)
    except urllib.error.URLError:
        return


def main():
    """ Main entry method. """
    vps = VpsUtils('Comics')
    creators = 'class="fancybox"'

    fetch_generic(vps, creators, 'Andy Capp',
                  'https://www.creators.com/read/andy-capp/')
    fetch_generic(vps, creators, 'B.C.', 'https://www.creators.com/read/bc')
    fetch_dilbert(vps)
    fetch_explosm(vps)
    fetch_comicskingdom(vps, 'Hagar',
                        'https://www.comicskingdom.com/hagar-the-horrible/')
    fetch_monkeyuser(vps)
    fetch_xkcd(vps)
    fetch_comicskingdom(vps, 'Zits', 'https://www.comicskingdom.com/zits')
    vps.deliver()


if __name__ == '__main__':
    main()
