""" events """
from collections import namedtuple
import json
from datetime import datetime, date, time
from dateutil.parser import parse
from sendmail import SendMail


_times = namedtuple('_times', 'nul now year days')
_event = namedtuple('_event', 'distance glyph text')


def handler(event, context):
    # pylint: disable=W0612,W0613
    """ Lambda function handler.
        Arguments:
            event - event from AWS
            context - execution context
    """
    Events().execute()

    return {
        'message': 'OK'
    }


class Events():
    """ Events backbone. """

    def __init__(self):
        """ Initialize class. """
        self.now = datetime.today()


    def execute(self):
        """ Events backbone handler. """
        title = self.title()
        page = self.head(title)
        all_times = self.build_time_constants()
        events = self.get_events(all_times)
        if events:
            events.sort(key=lambda tup: tup[0])

            html = ""
            for event in events:
                html += '<p style="font-size: 20px">'
                html += event.glyph + " " + event.text + "</p>"

            page = f'{page}<body>{html}</body></html>'
            SendMail().deliver(title, page)


    @staticmethod
    def title():
        """ Get title of mail. """
        now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        return f"Events {now}"


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
        with open("happenings.json", "r", encoding='utf8') as json_file:
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
        future = 7
        if yday < this_yday:
            if this_yday + future < yday + times.days:
                return None
            distance = yday + times.days - this_yday
        else:
            if this_yday + future < yday:
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
