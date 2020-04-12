""" kwed

Requirements:   feedparser>=5.1.3

Install with:
    pip3 install 'feedparser>=5.1.3'
"""
import os
import platform
import time
from urllib.parse import quote
from http.client import HTTPConnection
import feedparser
from vpsutils import VpsUtils


REMIX_HOST = 'remix.kwed.org'
HTML_ESCAPE_TABLE = {
    "&": "&amp;", '"': "&quot;", "'": "&apos;", ">": "&gt;", "<": "&lt;"
}


def html_escape(text):
    """ Produce entities within text.
        Args:
            text: source text.
    """
    return "".join(HTML_ESCAPE_TABLE.get(c, c) for c in text)


def get_counter_file(vps):
    """ Get location of counter file.
    	Arguments:
        	vps: VpsUtils
    """
    file = "_kwedcnt" if platform.system() == 'Windows' else ".kwedcnt"
    return os.path.join(vps.home, file)


def load_rss():
    """ Load feed. """
    all_music = {}

    rss = feedparser.parse('http://' + REMIX_HOST + '/rss.xml')
    if rss.bozo > 0:
        return all_music

    prefix = 'New C64 remix released: '
    prefix_len = len(prefix)
    for item in rss['items']:
        title = item['title']
        if title.startswith(prefix):
            title = title[prefix_len:]

        all_music[item['link']] = title + '.mp3'

    return all_music


def get_download_url(file_id):
    """ Get download link for link.
        Args:
            file_id: remix.kwed.org id
    """
    connection = HTTPConnection(REMIX_HOST)
    connection.request('GET', '/download.php/' + str(file_id))
    response = connection.getresponse()
    location = response.getheader('Location')
    if location is None:
        return None

    return 'http://' + REMIX_HOST + quote(location)


def send_email(vps, title, url):
    """ Send email with link to track.
        Args:
            vps: VpsUtils.
            title: title.
            url: link to track.
    """
    vps.html_add("""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
        <meta name="viewport" content="width=device-width" />
        <title>""")
    html_title = html_escape(title)
    timestamp = time.strftime('%Y-%m-%d %H:%M', time.localtime())
    vps.html_add('[KWED] ' + html_title + ' (' + timestamp + ')')
    vps.html_add("""</title>
</head>
<body><h3>""")
    vps.html_add(html_title)
    vps.html_add('</h3>\n<p>\n<a href="' + url + '">')
    vps.html_add(html_escape(url))
    vps.html_add("</a>\n</p>\n</body>\n")
    vps.deliver()
    vps.reset_html()


def main():
    """ Main loop. """
    vps = VpsUtils('KWED')
    counter_file = get_counter_file(vps)
    counter = 0
    with open(counter_file, 'r') as line:
        counter = int(line.read())

    feedparser.PREFERRED_XML_PARSERS.remove('drv_libxml2')
    feedparser.USER_AGENT = 'pykwed/1.2'
    music = load_rss()

    track_ids = []
    titles = {}
    for track_id in music:
        tid = int(track_id.split('=')[-1])
        titles[tid] = track_id
        track_ids.append(tid)

    track_ids.sort(key=int)

    for track_id in track_ids:
        if track_id <= counter:
            continue

        url = get_download_url(track_id)
        if url is None:
            continue

        send_email(vps, music[titles[track_id]], url)

        with open(counter_file, 'w') as data:
            data.write(str(track_id))


if __name__ == "__main__":
    main()
