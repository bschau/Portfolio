""" eventor """
from collections import namedtuple
import csv
from datetime import datetime, date, time
import os
import platform
from vpsutils import VpsUtils

FUTURE = 7
Times = namedtuple('Times', 'nul now year days')
Event = namedtuple('Event', 'distance glyph text')


def build_time_constants(vps):
    """ Build tuple with necessary time constants.
        Arguments:
            vps: VpsUtils
    """
    this_year = vps.now.tm_year
    return Times(time(0, 0, 0),
                 vps.now,
                 this_year,
                 (date(this_year + 1, 1, 1) - date(this_year, 1, 1)).days)


def get_events(vps, times):
    """ Load the events file.
        Arguments:
            vps: VpsUtils
            times: times tuple
    """
    file = '_events' if platform.system() == 'Windows' else '.events'
    full_path = os.path.join(vps.home, file)
    with open(full_path, 'r') as csv_file:
        events = []
        this_yday = times.now.tm_yday
        csv_reader = csv.reader(csv_file)

        for row in csv_reader:
            event = get_event(times, this_yday, row)
            if event is not None:
                events.append(event)

        return events


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
        text = "D&oslash;dsdag: {0} ({1} &aring;r siden)".format(row[4], age)

    if distance == 0:
        text = "<b>{0} i dag!</b>".format(text)
    else:
        if distance == 1:
            plural = ""
        else:
            plural = "e"

        text = "{0} om {1} dag{2}".format(text, distance, plural)

    return Event(distance, glyph, text)


def main():
    """ Main entry method. """
    vps = VpsUtils('Eventor')
    times = build_time_constants(vps)
    events = get_events(vps, times)
    if events:
        events.sort(key=lambda tup: tup[0])

        for event in events:
            vps.html_add('<p style="font-size: 20px">')
            vps.html_add(event.glyph + " " + event.text + "</p>")

        vps.deliver()


if __name__ == '__main__':
    main()
