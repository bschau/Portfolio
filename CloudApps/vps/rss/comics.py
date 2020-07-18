""" comics """
# Remember to:
#
#   pip3 install request
#   pip3 install rfeed
#
import datetime
import time
import urllib.request
from rfeed import Feed, Guid, Item


DST_URL = "https://leah.schau.dk/comics.rss"
DST_FILE = "/home/webs/leah/comics.rss"


def header(text, link):
    """ Add header.
        Args:
            text: header text
            link: link to comics site
    """
    html = '<h1><a href="{}">{}</a></h1>'.format(
        link, text)
    return html


def comics(html, image_link, alt_text, width=0):
    """ Add comics link to html.
        Args:
            html: existing html
            image_link: image data to inline
            alt_text: text to the 'alt' attribute
            width: optional width
    """
    html = '{}<p><img src="{}" alt="{}"'.format(html,
                                                image_link,
                                                alt_text)
    if width > 0:
        html = '{} width="{}"'.format(html, str(width))

    return "{} /></p>".format(html)


def line_break(html, break_text='<br />'):
    """ Add line break to HTML.
        Args:
            html: existing html
            break_text: the text between p / p
    """
    return '{}<p>{}</p>\r\n'.format(html, break_text)


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


def fetch_dilbert():
    """ Add the Dilbert comics. """
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
        return html
    except urllib.error.URLError:
        return ""


def fetch_monkeyuser():
    """ Add the Monkey User comics. """
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
        return html
    except urllib.error.URLError:
        return ""


def fetch_explosm():
    """ Add the Explosm comics. """
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
                return html

        pos = text.find("img id=\"main-comic\" src")
        start_pos = pos + 25
        end_pos = text.find('"', start_pos)
        link = '{}'.format(text[start_pos:end_pos])
        if link.startswith('//'):
            link = 'http:{}'.format(link)

        html = comics(html, link, 'Explosm', 900)
        html = line_break(html)
        return html
    except urllib.error.URLError:
        return ""


def fetch_xkcd():
    """ Add the XKCD comics. """
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
        return html
    except urllib.error.URLError:
        return ""


def generic(origin, token, title, url):
    """ Add the generic comics.
        Args:
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
        return html
    except urllib.error.URLError:
        return ""


def fetch_ingemann():
    """ Fetch the Ingemann comic from Ekstrabladet. """
    try:
        ebroot = "https://ekstrabladet.dk"
        text = fetch(ebroot + '/ingemann/')
        search = "flex-item mar-l--b"
        pos = text.find(search)
        if pos < 0:
            return ""

        search = "a href"
        start_pos = text.find(search, pos) + len(search) + 2
        end_pos = text.find('"', start_pos)
        url = ebroot + text[start_pos:end_pos]
        text = fetch(url)
        search = "image-container"
        pos = text.find(search)
        if pos < 0:
            return ""

        search = "src"
        start_pos = text.find(search, pos) + len(search) + 2
        end_pos = text.find('"', start_pos)
        title = "Ingemann"
        link = ebroot + text[start_pos:end_pos]
        html = header(title, url)
        html = comics(html, link, title, 900)
        html = line_break(html)
        return html
    except urllib.error.URLError:
        return ""


def get_feed(feed):
    """ Get the atom ready feed.
        Arguments:
            feed - the rfeed.
    """
    feed = feed.replace(
        "<rss version=\"2.0\"",
        "<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\"")
    atomfmt = "<channel><atom:link href=\"{}\" rel=\"{}\" type=\"{}\" />"
    atom = atomfmt.format(DST_URL, "self", "application/rss+xml")
    feed = feed.replace("<channel>", atom)
    return feed


def main():
    """ Main loop. """
    c_origin = 'class="fancybox"'
    c_token = 'img src'
    ck_origin = False
    ck_token = 'data-comic-image-url'

    url = 'https://www.creators.com/read/andy-capp'
    html = ""
    html = html + generic(c_origin, c_token, 'Andy Capp', url)

    url = 'https://www.creators.com/read/bc'
    html = html + generic(c_origin, c_token, 'B.C.', url)

    html = html + fetch_dilbert()
    html = html + fetch_explosm()

    url = 'https://www.comicskingdom.com/hagar-the-horrible/'
    html = html + generic(ck_origin, ck_token, 'Hagar', url)

    html = html + fetch_ingemann()
    html = html + fetch_monkeyuser()
    html = html + fetch_xkcd()

    url = 'https://www.comicskingdom.com/zits'
    html = html + generic(ck_origin, ck_token, 'Zits', url)

    now = datetime.datetime.utcnow()
    text = "Comics {}/{}/{}".format(now.day, now.month, now.year)
    item = Item(
        title=text,
        link=DST_URL,
        description=html,
        guid=Guid("{}?ts={}".format(DST_URL, time.time())),
        pubDate=datetime.datetime.utcnow()
    )

    text = "Favorite comics {}/{}/{}".format(now.day, now.month, now.year)
    feed = Feed(
        title="Comics",
        link=DST_URL,
        description=text,
        language="en-US",
        lastBuildDate=datetime.datetime.utcnow(),
        ttl=720,
        items=[item]
    )

    with open(DST_FILE, 'w') as rss_file:
        rss_file.write(get_feed(feed.rss()))


if __name__ == "__main__":
    main()
