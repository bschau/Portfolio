""" trellotodo """
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import json
import os
import platform
from datetime import datetime
import time
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import requests


class TrelloTodo():
    """ TrelloTodo backbone. """
    BASE_URL = 'https://api.trello.com/1'

    def __init__(self):
        """ Setup for execution. """
        # 'apikey token board'
        config = self.get_rc().split(' ')
        self.api_key = config[0]
        self.token = config[1]
        self.board_name = config[2]


    @staticmethod
    def get_rc():
        """ Load rcfile. """
        home = os.path.expanduser("~")
        name = "_trellorc" if platform.system() == 'Windows' else ".trellorc"
        token_file = os.path.join(home, name)
        with open(token_file, encoding='utf8') as token:
            return token.readline().strip()


    def execute(self):
        """ TrelloTodo runner. """
        board_id = self.get_board_id(self.board_name)
        lists = self.get_lists_for_board(board_id)
        cards = self.get_cards(board_id)
        if cards:
            html = self.create_html(cards, lists)
            title = self.title()
            body = self.head(title)
            body = f'{body}<body><table>'
            body = f'{body}<tr><th style="text-align: left">List</th>'
            body = f'{body}<th style="text-align: left">Task</th>'
            body = f'{body}<th style="text-align: left">Due date</th></tr>'
            body = f'{body}{html}</table></body></html>'
            self.deliver(title, body)


    def get_board_id(self, board_name):
        """ Get board_id from board_name.
            Args:
                board_name: name of board to resolve.
        """
        url = f"{self.BASE_URL}/members/me/boards?key={self.api_key}"
        url = f"{url}&token={self.token}&filter=open"
        response = requests.request("GET", url)
        if response is None:
            return None

        boards = json.loads(response.text)
        for board in boards:
            if board['name'] == board_name:
                return board['id']

        return None


    def get_lists_for_board(self, board_id):
        """ Get lists on board.
            Args:
                board_id: board id.
        """
        if board_id is None:
            return None

        url = f"{self.BASE_URL}/boards/{board_id}/lists?key={self.api_key}"
        url = f"{url}&token={self.token}&filter=open"
        response = requests.request("GET", url)
        lists = json.loads(response.text)
        trello_lists = {}
        for lst in lists:
            trello_lists[lst['id']] = lst['name']

        return trello_lists


    def get_cards(self, board_id):
        """ Get cards on board.
            Args:
                board_id: board id.
        """
        url = f"{self.BASE_URL}/boards/{board_id}/cards?key={self.api_key}"
        url = f"{url}&token={self.token}"
        response = requests.request("GET", url)
        cards = json.loads(response.text)

        sorted_cards = []
        unsorted_cards = []
        for card in cards:
            if card['due'] is None:
                unsorted_cards.append(card)
            else:
                sorted_cards.append(card)

        sorted_cards.sort(key=lambda x: datetime.strptime(x['due'],
                          "%Y-%m-%dT%H:%M:%S.%f%z"))
        return {
            'sorted': sorted_cards,
            'unsorted': unsorted_cards
        }


    @staticmethod
    def create_html(cards, lists):
        """ Create the HTML page.
            Args:
                cards: dict with cards.
                lists: lists.
        """
        html = ""
        link_formatter = "<a href=\"{}\">{}</a>"

        formatter = ('<tr><td style="padding-right: 1em">{}</td>'
                     '<td style="padding-right: 1em">{}</td><td>{}</tr>')
        for card in cards['sorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            due = card['due'][0:10] + " (" + card['due'][11:19] + ")"
            line = formatter.format(lists[card['idList']], link, due)
            html = f"{html}{line}"

        for card in cards['unsorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            line = formatter.format(lists[card['idList']], link, "")
            html = f"{html}{line}"

        return html


    @staticmethod
    def title():
        """ Get title of mail. """
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        return f"Trello Todo {now}"


    @staticmethod
    def head(title):
        """ Get HTML head (of page).
            Arguments:
                title - title.
        """
        return f"""<!DOCTYPE html>
<html dir="ltr" lang="en">
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
    <meta name="viewport" content="width=device-width" />
    <title>{title}</title>
</head>"""


    @staticmethod
    def deliver(title, body):
        """ Delivery email by SMTP.
            Arguments:
                title: mail title
                body: body text
        """
        text = "Please read this in a HTML mail user agent."
        recipients = ["bschau@posteo.net"]
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
    TrelloTodo().execute()
