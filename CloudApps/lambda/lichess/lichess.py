""" lichess """
import json
import os
import time
import urllib
from mailgun import MailGun


class Lichess():
    """ Lichess backbone. """

    def execute(self):
        """ Lichess runner. """
        token = os.environ['LICHESS_TOKEN']
        jsondata = self.fetch_overview(token)

        if jsondata is not None:
            overview = json.loads(jsondata.decode('utf-8'))
            games = self.get_my_turn_games(overview)
            if games:
                html = self.games_to_html(games)
                title = self.title()
                page = self.page(title).format(html)
                MailGun('Lichess').deliver(title, page)


    @staticmethod
    def fetch_overview(token):
        """ Fetch overview of ongoing games.
            Args:
    	        token: lichess token
        """
        try:
            url = 'https://lichess.org/api/account/playing'
            request = urllib.request.Request(
                url, data=None,
                headers={
                    'User-Agent': ('Mozilla/5.0 (X11; Ubuntu; Linux x86_64;'
                                   'rv:55.0) Gecko/20100101 Firefox/55.0'),
                    'Authorization': 'Bearer ' + token
                }
            )
            return urllib.request.urlopen(request).read()
        except urllib.error.URLError:
            return None


    @staticmethod
    def get_my_turn_games(overview):
        """ Get the games in which I am in turn.
            Args:
                overview: the oveview json file
        """
        games = []
        for game in overview["nowPlaying"]:
            if game["isMyTurn"]:
                games.append(game)

        return games


    @staticmethod
    def title():
        """ Get title of mail. """
        return "Lichess {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
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
<p>My turn in game vs.:</p>
<ul>
{{0}}
</ul>
</body>
</html>""".format(title)


    @staticmethod
    def games_to_html(games):
        """ Create HTML from games.
            Args:
                games: games in which I am in turn
        """
        html = ""
        formatter = "<li><i>{}</i>: {}</li>"
        linkformatter = "<a href=\"https://lichess.org/{}\">Go to game</a>"
        for game in games:
            opponent = game["opponent"]["username"]
            link = linkformatter.format(game["fullId"])
            line = formatter.format(opponent, link)
            html = "{0}{1}".format(html, line)

        return html
