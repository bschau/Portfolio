""" kwed

Install with:
    pip3 install feedgen
    pip3 install feedparser
    pip3 install requests
"""
from urllib.parse import quote
from http.client import HTTPSConnection
import requests
import feedparser
from feedgen.feed import FeedGenerator


class Kwed():
    """ Kwed backbone. """

    def execute(self):
        """ Run the handler. """
        feedparser.USER_AGENT = 'kwed/6.0'
        rss = feedparser.parse('https://remix.kwed.org/rss.xml')
        feed = FeedGenerator()
        feed.id('https://leah.schau.dk/kwed.xml')
        feed.title('KWED')
        feed.author({'name':'Jan Lund Thomsen', 'email':'remix.kwed.org@gmail.com'})
        feed.link(href='https://remix.kwed.org/', rel='alternate')
        feed.logo('https://leah.schau.dk/rko.jpg')
        feed.subtitle('Remix.Kwed.Org')
        feed.link(href='https://leah.schau.dk/kwed.xml', rel='self')
        feed.language('en')
        feed.load_extension('podcast')
        # pylint: disable=no-member
        feed.podcast.itunes_category('Music', 'Music History')

        for item in rss['items']:
            tid = int(item['link'].split('/')[-1])
            url = self.get_download_url(tid)
            if url is None:
                print(str(tid) + ": no download url")
                continue

            content_type, length = self.get_content_metadata(url)
            if not type:
                print(url + ': no response to HEAD')
                continue

            entry = feed.add_entry()
            entry.id(item['id'])
            entry.title(self.get_title(item['title']))
            entry.description(item['summary'])
            entry.enclosure(url, length, content_type)
            entry.published(item['published'])

        feed.rss_str(pretty=True)
        feed.rss_file('/home/webs/leah/kwed.xml')


    @staticmethod
    def get_download_url(file_id):
        """ Get download link for link.
            Arguments:
                file_id: remix.kwed.org id
        """
        connection = HTTPSConnection('remix.kwed.org')
        connection.request('HEAD', '/download.php/' + str(file_id))
        response = connection.getresponse()
        location = response.getheader('Location')
        if location is None:
            return None

        return 'https://remix.kwed.org' + quote(location)


    def get_content_metadata(self, url):
        """ Get content type / length for track.
            Arguments:
                url: url to track
        """
        response = requests.head(url)
        if response is None:
            return False, False

        content_type = self.get_header(response, 'Content-Type', 'audio/mpeg')
        length = self.get_header(response, 'Content-Length', 0)
        return content_type, length


    @staticmethod
    def get_header(response, key, value):
        """ Get value of response header or default value.
            Arguments:
                response: response object
                key: header key
                value: default value
        """
        if not response.headers[key]:
            return value

        return response.headers[key]


    @staticmethod
    def get_title(title):
        """ Remove prefix from title.
            Arguments:
                title: track title
        """
        prefix = 'New C64 remix released: '
        prefix_len = len(prefix)

        if title.startswith(prefix):
            return title[prefix_len:]

        return title


if __name__ == '__main__':
    Kwed().execute()
