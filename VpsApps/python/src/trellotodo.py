""" trellotodo """
import json
import os
import platform
from datetime import datetime
import time
import requests
from sendmail import SendMail


class TrelloTodo():
    """ TrelloTodo backbone. """
    BASE_URL = 'https://api.trello.com/1'

    def __init__(self):
        """ Setup for execution. """
        home = os.path.expanduser("~")
        filename = "_trello" if platform.system() == 'Windows' else ".trello"
        trello_file = os.path.join(home, filename)
        with open(trello_file) as json_file:
            content = json.load(json_file)
            self.api_key = content["api_key"]
            self.token = content["token"]
            self.board_name = content["board"]


    def execute(self):
        """ TrelloTodo runner. """
        board_id = self.get_board_id(self.board_name)
        lists = self.get_lists_for_board(board_id)
        cards = self.get_cards(board_id)

        html = self.create_html(cards, lists)
        title = self.title()
        body = self.page(title).format(html)
        SendMail('TrelloTodo').deliver(title, body)


    def get_board_id(self, board_name):
        """ Get board_id from board_name.
            Args:
                board_name: name of board to resolve.
        """
        url = "{0}/members/me/boards?key={1}&token={2}&{3}".format(
            self.BASE_URL, self.api_key, self.token, 'filter=open'
        )
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

        url = "{0}/boards/{1}/lists?key={2}&token={3}&filter=open".format(
            self.BASE_URL, board_id, self.api_key, self.token
        )

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
        url = "{0}/boards/{1}/cards?key={2}&token={3}".format(
            self.BASE_URL, board_id, self.api_key, self.token
        )

        response = requests.request("GET", url)
        cards = json.loads(response.text)

        sorted_cards = []
        unsorted_cards = []
        for card in cards:
            if card['due'] is None:
                unsorted_cards.append(card)
            else:
                sorted_cards.append(card)

        sorted_cards.sort(key=lambda x: datetime.strptime(x['due'], "%Y-%m-%dT%H:%M:%S.%f%z"))
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

        formatter = "<tr><td>{}</td><td>{}</td><td>{}</tr>"
        for card in cards['sorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            due = card['due'][0:10] + " (" + card['due'][11:19] + ")"
            line = formatter.format(lists[card['idList']], link, due)
            html = "{}{}".format(html, line)

        for card in cards['unsorted']:
            link = link_formatter.format(card['shortUrl'], card['name'])
            line = formatter.format(lists[card['idList']], link, "")
            html = "{}{}".format(html, line)

        return html


    @staticmethod
    def title():
        """ Get title of mail. """
        return "Trello Todo {0}".format(time.strftime('%Y-%m-%d %H:%M:%S',
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
<table>
<tr>
    <th>List</th>
    <th>Task</th>
    <th>Due date</th>
</tr>
{{0}}
</table>
</body>
</html>""".format(title)


if __name__ == '__main__':
    TrelloTodo().execute()
