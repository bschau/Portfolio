""" lichessdigest """
import json
import os
import platform
import urllib
from vpsutils import VpsUtils


def get_lichess_token():
    """ Get token from file. """
    file = "_lichesst" if platform.system() == 'Windows' else ".lichesst"
    home = os.path.expanduser("~")
    tokenfile = os.path.join(home, file)
    with open(tokenfile, 'r') as line:
        return line.read().strip()


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
        response = urllib.request.urlopen(request)
        data = response.read()
        return data
    except urllib.error.URLError:
        return None


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


def games_to_html(vps, games):
    """ Add the games to html
        Args:
            vps: VpsUtils
            games: games in which I am in turn
    """
    formatter = "<li><i>{}</i>: {}</li>"
    linkformatter = "<a href=\"https://lichess.org/{}\">Go to game</a>"
    for game in games:
        opponent = game["opponent"]["username"]
        link = linkformatter.format(game["fullId"])
        vps.html_add(formatter.format(opponent, link))


def main():
    """ Main entry method. """
    vps = VpsUtils('Lichess')
    vps.html_add("<p>My turn in game vs.:</p>")
    vps.html_add("<ul>")

    token = get_lichess_token()
    jsondata = fetch_overview(token)
    if jsondata is not None:
        overview = json.loads(jsondata.decode('utf-8'))
        games = get_my_turn_games(overview)
        if games:
            games_to_html(vps, games)
            vps.html_add("</ul>")
            vps.deliver()


if __name__ == '__main__':
    main()
