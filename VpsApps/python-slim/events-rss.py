""" events """
from collections import namedtuple
import datetime
import json
import os
import platform
from email import utils
from dateutil.parser import parse
from feedgen.feed import FeedGenerator


_times = namedtuple('_times', 'nul now year days')
_event = namedtuple('_event', 'distance glyph text')


class Events():
    """ Events backbone. """

    def __init__(self):
        """ Initialize class. """
        self.now = datetime.datetime.today()


    def execute(self):
        """ Events backbone handler. """
        base = 'https://nop.ldx.dk'
        nowdt = datetime.datetime.now()
        now = nowdt.strftime('%Y-%m-%d')

        feed = FeedGenerator()
        feed.id(f"{base}/events.xml")
        feed.title('Events')
        feed.author({'name':'Brian Schau', 'email':'brian@schau.dk'})
        feed.subtitle('Events')
        feed.link(href=f"{base}/events.xml", rel='self')
        feed.language('en')
        feed.ttl(60)

        all_times = self.build_time_constants()
        events = self.get_events(all_times)
        if events:
            events.sort(key=lambda tup: tup[0])

            html = ""
            for event in events:
                html += '<p style="font-size: 20px">'
                html += event.glyph + " " + event.text + "</p>"

            entry = feed.add_entry()
            entry.id(str(now))
            entry.title(f"Events {now}")
            entry.description(html)
            entry.published(utils.format_datetime(nowdt))

        feed.rss_str(pretty=True)
        feed.rss_file('/home/webs/nop/events.xml')


    def build_time_constants(self):
        """ Build tuple with necessary time constants. """
        this_year = self.now.year
        date_start = datetime.date(this_year + 1, 1, 1)
        date_end = datetime.date(this_year, 1, 1)
        return _times(datetime.time(0, 0, 0),
                      self.now,
                      this_year,
                      (date_start - date_end).days)


    def get_events(self, times):
        """ Load the events file.
            Arguments:
                times: times tuple
        """
        events = []
        this_yday = self.now.timetuple().tm_yday
        file = '_eventsrc' if platform.system() == 'Windows' else '.eventsrc'
        home = os.path.expanduser('~')
        full_path = os.path.join(home, file)
        with open(full_path, "r", encoding='utf8') as json_file:
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
        event_date = datetime.date(times.year, source_date.month,
                                   source_date.day)
        full_time = datetime.datetime.combine(event_date, times.nul)
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
