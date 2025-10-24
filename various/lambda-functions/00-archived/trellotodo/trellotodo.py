""" trellotodo """
import json
import os
from datetime import datetime
import time
import requests
from sendmail import SendMail


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    TrelloTodo().execute()

    return {
        'message': 'OK'
    }


class TrelloTodo():
    """ TrelloTodo backbone. """
    BASE_URL = 'https://api.trello.com/1'

    def __init__(self):
        """ Setup for execution. """
        self.api_key = os.environ["TRELLO_APIKEY"]
        self.token = os.environ["TRELLO_TOKEN"]
        self.board_name = os.environ["TRELLO_BOARD"]


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
            body = f'{body}<th style="text-align: left">Due date</th>'
            body = f'{body}<th style="text-align: left">Notes</th></tr>'
            body = f'{body}{html}</table></body></html>'
            SendMail().deliver(title, body)


    def get_board_id(self, board_name):
        """ Get board_id from board_name.
            Args:
                board_name: name of board to resolve.
        """
        url = f"{self.BASE_URL}/members/me/boards?key={self.api_key}"
        url = f"{url}&token={self.token}&filter=open"
        response = requests.request("GET", url, timeout=30)
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
        response = requests.request("GET", url, timeout=30)
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
        response = requests.request("GET", url, timeout=30)
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
                     '<td style="padding-right: 1em">{}</td><td>{}</td>'
                     '<td>{}</td></tr>')
        for card in cards['sorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            due = card['due'][0:10] + " (" + card['due'][11:19] + ")"
            line = formatter.format(lists[card['idList']],
                                    link,
                                    due,
                                    card['desc'])
            html = f"{html}{line}"

        for card in cards['unsorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            line = formatter.format(lists[card['idList']],
                                    link,
                                    "",
                                    card['desc'])
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


if __name__ == '__main__':
    TrelloTodo().execute()
