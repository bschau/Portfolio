""" newsfeed """
from datetime import datetime, timedelta
import dbm
import os
import json
import platform
import time
import feedparser
from vpsutils import VpsUtils


feedparser.USER_AGENT = 'newsfeed/3.0'


def get_cache_file(vps):
    """ Get the cache file.
        Arguments:
            vps: VpsUtils
    """
    file = '_newsfeedT' if platform.system() == 'Windows' else '.newsfeedT'
    full_path = os.path.join(vps.home, file)
    return dbm.open(full_path, 'c')


def get_feeds(vps):
    """ Get the feeds file.
        Arguments:
            vps: VpsUtils
    """
    file = '_newsfeedrc' if platform.system() == 'Windows' else '.newsfeedrc'
    full_path = os.path.join(vps.home, file)
    with open(full_path, encoding='utf-8') as json_file:
        return json.load(json_file)


def can_cope_with_bozo(bozo_exception):
    """ Can we copy with the bozo exception?
        Args:
            bozo_exception: exception
    """
    bozo_exception = bozo_exception.lower()
    if 'document declared a~s' in bozo_exception:
        return True

    if 'is not an xml media type' in bozo_exception:
        return True

    if 'no content-type specified' in bozo_exception:
        return True

    return False


def load_feed(vps, cache, line_number, tag, url):
    """ Main method.
        Args:
            vps: VpsUtils
            cache: cache db
            line_number: current line number
            tag: short preamble
            url: feed url
    """
    rss = feedparser.parse(url)
    if rss.bozo > 0:
        if not can_cope_with_bozo(str(rss.bozo_exception)):
            return line_number

    cache_time = time.strftime('%Y/%m/%d %H:%M:%S', vps.now)
    for item in rss['items']:
        link = item['link']
        if cache.get(link, '') == '':
            line_number = add_news(vps, line_number, tag, item['title'], link)
            cache[link] = cache_time

    return line_number


def expire(cache):
    """ Remove old entries and close database.
        Arguments:
            cache: cache db
    """
    keys = cache.keys()
    cutoff = datetime.now() - timedelta(365)
    for key in keys:
        decoded = cache[key].decode('utf-8')
        key_date = datetime.strptime(decoded, '%Y/%m/%d %H:%M:%S')
        if key_date >= cutoff:
            continue

        del cache[key]

    cache.close()


def add_news(vps, line_number, tag, title, url):
    """ Add a newsfeed line to mail.
        Args:
            vps: VpsUtils
            line_number: current line number
            tag: tag
            title: title of news
            url: deep link to article
    """
    color = '#efe' if line_number % 2 == 1 else '#fff'
    title = '(' + tag + ') ' + title
    vps.html_add('<tr><td style="background-color: ')
    vps.html_add(color)
    vps.html_add('; padding: 0.5em; font-size: 120%"><a href="')
    vps.html_add(url)
    vps.html_add('">' + title + '</a></td></tr>\r\n')
    line_number = line_number + 1
    return line_number


def main():
    """ Main entry method. """
    vps = VpsUtils('News')
    cache = get_cache_file(vps)

    vps.html_add('<table style="width: 100%">')
    line_number = 0
    feeds = get_feeds(vps)

    for feed in feeds:
        line_number = load_feed(vps, cache, line_number,
                                feed["Name"],
                                feed["Url"])

    vps.html_add('</table>')

    if line_number > 0:
        vps.deliver()

    expire(cache)


if __name__ == '__main__':
    main()
