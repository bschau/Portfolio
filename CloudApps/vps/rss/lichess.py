""" lichess """
# Remember to:
#
#   pip3 install request
#   pip3 install rfeed
#
import datetime
import json
import os
import platform
import time
import urllib.request
from rfeed import Feed, Guid, Item


DST_URL = "https://leah.schau.dk/lichess.rss"
DST_FILE = "/home/webs/leah/lichess.rss"


def get_token():
    """ Read token from file. """
    home = os.path.expanduser("~")
    file = '_lichesst' if platform.system() == 'Windows' else '.lichesst'
    full_path = os.path.join(home, file)
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


def games_to_items(games):
    """ Create items array from games.
        Args:
            games: games in which I am in turn
    """
    result = []
    urlformatter = "https://lichess.org/{}"
    textformatter = "<a href=\"{}\">Go to game</a>"
    guidformatter = "{}?ts={}"
    for game in games:
        url = urlformatter.format(game["fullId"])
        text = textformatter.format(url)
        item = Item(
            title="Game vs. " + game["opponent"]["username"],
            link=url,
            description=text,
            guid=Guid(guidformatter.format(url, time.time())),
            pubDate=datetime.datetime.utcnow()
        )
        result.append(item)

    return result


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
    token = get_token()
    jsondata = fetch_overview(token)

    if jsondata is not None:
        overview = json.loads(jsondata.decode('utf-8'))
        games = get_my_turn_games(overview)
        if games:
            items = games_to_items(games)
            feed = Feed(
                title="Lichess",
                link=DST_URL,
                description="Lichess - the wonderful chess site",
                language="en-US",
                lastBuildDate=datetime.datetime.utcnow(),
                ttl=60,
                items=items
            )

            with open(DST_FILE, 'w') as rss_file:
                rss_file.write(get_feed(feed.rss()))


if __name__ == "__main__":
    main()
