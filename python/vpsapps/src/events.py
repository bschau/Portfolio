""" events """
from collections import namedtuple
import json
import os
import platform
from datetime import datetime, date, time
from dateutil.parser import parse
from sendmail import SendMail


FUTURE = 7
_times = namedtuple('_times', 'nul now year days')
_event = namedtuple('_event', 'distance glyph text')


class Events():
    """ Events backbone. """

    def __init__(self):
        """ Initialize class. """
        self.now = datetime.today()


    def execute(self):
        """ Events backbone handler. """
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

            SendMail('Events').deliver(title, page.format(html))


    @staticmethod
    def title():
        """ Get title of mail. """
        now = datetime.now()
        return "Events {0}".format(now.strftime('%Y-%m-%d %H:%M:%S'))


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
        events = []
        this_yday = self.now.timetuple().tm_yday
        home = os.path.expanduser("~")
        filename = "_events" if platform.system() == 'Windows' else ".events"
        events_file = os.path.join(home, filename)
        with open(events_file) as json_file:
            event_file = json.load(json_file)
            for source in event_file["Events"]:
                event = self.get_event(times, this_yday, source, event_file)
                if event is not None:
                    events.append(event)

        return events


    def get_event(self, times, this_yday, event, event_file):
        """ Return tuple with event.
            Arguments:
                times: times tuple
                this_yday: this day in year
                event: current event
                event_file: entire event file
        """
        source_date = parse(event["Date"])
        event_date = date(times.year, source_date.month, source_date.day)
        full_time = datetime.combine(event_date, times.nul)
        distance = self.get_distance(full_time.timetuple().tm_yday,
                                     this_yday,
                                     times)
        if distance is None:
            return None

        event_type = event["Type"]
        glyph = event_file["Icons"][event_type]
        texts = event_file["Texts"]
        age = times.year - source_date.year
        text = (texts[event_type]).format(event["Description"], age)
        if distance == 0:
            return _event(0, glyph, texts["Today"].format(text))

        text = self.get_event_text(distance, texts, text)
        return _event(distance, glyph, text)


    @staticmethod
    def get_distance(yday, this_yday, times):
        """ Calculate distance from now - looking FUTURE
            days into the future.
            Arguments:
                yday: day in year of event.
                this_yday: this day in year.
                times: times array.
        """
        distance = 0
        if yday < this_yday:
            if this_yday + FUTURE < yday + times.days:
                return None
            distance = yday + times.days - this_yday
        else:
            if this_yday + FUTURE < yday:
                return None
            distance = yday - this_yday

        return distance

    @staticmethod
    def get_event_text(distance, texts, text):
        """ Get the event text with possible pluralization.
            Arguments:
                distance: distance > 0.
                texts: texts array.
                text: event text.
        """
        if distance == 1:
            plural = ""
        else:
            plural = texts["Pluralis"]

        return texts["InDays"].format(text, distance, plural)


if __name__ == '__main__':
    Events().execute()
