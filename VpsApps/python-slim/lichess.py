""" lichess

Requirements: requests
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import json
import time
import urllib.request
import os
import platform
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText


class Lichess():
    """ Lichess backbone. """

    def execute(self):
        """ Lichess runner. """
        token = self.get_token()
        jsondata = self.fetch_overview(token)

        if jsondata is not None:
            overview = json.loads(jsondata.decode('utf-8'))
            games = self.get_my_turn_games(overview)
            if games:
                html = self.games_to_html(games)
                title = self.title()
                page = self.page_header(title)
                page = f"{html}</ul></body></html>"
                self.deliver(title, page)


    @staticmethod
    def get_token():
        """ Load token from file. """
        home = os.path.expanduser("~")
        name = "_lichesst" if platform.system() == 'Windows' else ".lichesst"
        token_file = os.path.join(home, name)
        with open(token_file, encoding='utf8') as token:
            return token.readline().strip()


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
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        return f"Lichess {now}"


    @staticmethod
    def page_header(title):
        """ Get HTML page header.
            Arguments:
                title - title.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>
<body>
<p>My turn in game vs.:</p>
<ul>"""



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
            html = f"{html}{line}"

        return html


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["brian@schau.dk"]
        message = MIMEMultipart('alternative')
        message['Subject'] = title
        message['From'] = "bs@leah.schau.dk"
        message['To'] = ",".join(recipients)
        plain_text = MIMEText(text, 'plain')
        html_text = MIMEText(body, 'html')
        message.attach(plain_text)
        message.attach(html_text)

        mail = smtplib.SMTP('localhost')
        mail.sendmail(message['From'], recipients, message.as_string())
        mail.quit()


if __name__ == '__main__':
    Lichess().execute()
