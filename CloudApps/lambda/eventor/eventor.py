""" eventor """
from collections import namedtuple
import csv
from datetime import datetime, date, time
from mailgun import MailGun

FUTURE = 7
_times = namedtuple('_times', 'nul now year days')
_event = namedtuple('_event', 'distance glyph text')


class Eventor():
    """ Eventor backbone. """

    def __init__(self):
        """ Initialize class. """
        self.now = datetime.today()


    def execute(self):
        """ Eventor backbone handler. """
        title = self.title()
        page = self.page(title)
        all_times = self.build_time_constants()
        events = self.get_events(all_times)
        if events:
            events.sort(key=lambda tup: tup[0])

            html = ""
            for event in events:
                html += '<p style="font-size: 20px">'
                html += event.glyph + " " + event.text + "</p>"

            MailGun('Eventor').deliver(title, page.format(html))


    @staticmethod
    def title():
        """ Get title of mail. """
        now = datetime.now()
        return "Eventor {0}".format(now.strftime('%Y-%m-%d %H:%M:%S'))


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
{{0}}
</body>
</html>""".format(title)


    def build_time_constants(self):
        """ Build tuple with necessary time constants. """
        this_year = self.now.year
        return _times(time(0, 0, 0),
                      self.now,
                      this_year,
                      (date(this_year + 1, 1, 1) - date(this_year, 1, 1)).days)


    def get_events(self, times):
        """ Load the events file.
            Arguments:
                times: times tuple
        """
        with open('events.txt', 'r') as csv_file:
            events = []
            this_yday = self.now.timetuple().tm_yday
            csv_reader = csv.reader(csv_file)

            for row in csv_reader:
                event = self.get_event(times, this_yday, row)
                if event is not None:
                    events.append(event)

            return events


    @staticmethod
    def get_event(times, this_yday, row):
        """ Return tuple with event.
            Arguments:
                times: times tuple
                this_yday: this day in year
                row: event row
        """
        event_date = date(times.year, int(row[1]), int(row[2]))
        full_time = datetime.combine(event_date, times.nul)
        yday = full_time.timetuple().tm_yday
        distance = 0

        if yday < this_yday:
            if this_yday + FUTURE < yday + times.days:
                return None
            distance = yday + times.days - this_yday
        else:
            if this_yday + FUTURE < yday:
                return None
            distance = yday - this_yday

        age = times.year - int(row[0])
        if row[3] == "F":
            glyph = "&#127874;"
            text = "F&oslash;dselsdag: {0} ({1} &aring;r)".format(row[4], age)
        elif row[3] == "D":
            glyph = "&#10013;"
            text = "D&oslash;dsdag: {0} ({1} &aring;r siden)".format(row[4],
                                                                     age)

        if distance == 0:
            text = "<b>{0} i dag!</b>".format(text)
        else:
            if distance == 1:
                plural = ""
            else:
                plural = "e"

            text = "{0} om {1} dag{2}".format(text, distance, plural)

        return _event(distance, glyph, text)
