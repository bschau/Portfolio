""" test """
import time
import urllib
import urllib.error
import urllib.request


class Test():
    """ Test backbone. """

    def execute(self):
        """ Run the handler. """
        return {
            "Time": self.get_time(),
            "Message" : self.get_message()
        }


    @staticmethod
    def get_time():
        """ Return now. """
        return "{0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
                                          time.localtime()))


    @staticmethod
    def get_message():
        """ Load test.txt out of schau.dk. """
        try:
            url = "https://schau.dk/test.txt"
            request = urllib.request.Request(
                url, data=None,
                headers={
                    'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                                   'rv:55.0) Gecko/20100101 Firefox/55.0')
                }
            )
            data = urllib.request.urlopen(request).read()
            return '{}'.format(data)
        except urllib.error.URLError:
            return ""
