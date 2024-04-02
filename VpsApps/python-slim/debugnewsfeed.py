""" debug-newsfeed

Requirements: feedparser, request
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import sys
import feedparser
import requests


USER_AGENT = "blogtrottr/2.0"
RSS_SITE = "https://meremobil.dk/feed"


headers = {'User-Agent': USER_AGENT, 'Connection': 'close'}
print(f"Fetching from {RSS_SITE} ...")
try:
    data = requests.get(RSS_SITE,
                        headers=headers,
                        timeout=30).content.strip()
except requests.exceptions.Timeout:
    print('Timeout!')
    sys.exit(1)
except requests.exceptions.RequestException as req_exc:
    print('Exception: ' + req_exc)
    sys.exit(1)

rss = feedparser.parse(data)
print("RSS returned:")
print(rss)
if rss.bozo > 0:
    BOZO_EXC = str(rss.bozo_exception)
    if not can_cope_with_bozo(BOZO_EXC):
        print(f'Fetch error: {BOZO_EXC}')
        sys.exit(1)

print()
print("Entries:")
for item in rss['items']:
    print(item['title'])
    print(f"\t{item['link']}")


def can_cope_with_bozo(bozo_exception):
    """ Can we copy with the bozo exception?
        Args:
            bozo_exception: exception
    """
    bozo_exception = bozo_exception.lower()
    if 'document declared as' in bozo_exception:
        return True

    if 'is not an xml media type' in bozo_exception:
        return True

    if 'no content-type specified' in bozo_exception:
        return True

    return False
