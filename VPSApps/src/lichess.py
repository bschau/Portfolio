""" lichess """
import json
import os
import platform
import urllib
from vpsutils import VpsUtils


def get_token(vps):
    """ Read token from file.
        Args:
            vps: VpsUtils
    """
    file = '_lichesst' if platform.system() == 'Windows' else '.lichesst'
    full_path = os.path.join(vps.home, file)
    with open(full_path, 'r') as lichesst:
        return lichesst.read().rstrip('\n')


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


def main():
    """ Lichess main method. """
    vps = VpsUtils('Lichess')
    token = get_token(vps)
    jsondata = fetch_overview(token)
    if jsondata is not None:
        overview = json.loads(jsondata.decode('utf-8'))
        games = get_my_turn_games(overview)
        if games:
            html = games_to_html(games)
            vps.html_add('<p>My turn in game vs.:</p><ul>')
            vps.html_add(html)
            vps.html_add('</ul>')
            vps.deliver()


if __name__ == '__main__':
    main()
