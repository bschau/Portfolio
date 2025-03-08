#!/usr/bin/env python3
""" mkpage.py - generate index page. """
import base64
import csv


def get_base64_file(slug):
    """ Read image and return base64.
        Arguments:
            slug - image slug.
    """
    with open('img/' + slug + '.png', "rb") as img:
        encoded_img = base64.b64encode(img.read())
        return encoded_img.decode()


def build_entry(title, img, url):
    """ Build html entry.
        Arguments:
            title - title text.
            img - image slug.
            url - url to open.
    """
    html = '<div class="e">'
    html = html + '<a href="' + url + '">'
    html = html + '<img src="data:image/png;base64,'
    html = html + get_base64_file(img)
    html = html + '" alt="' + title + '" width="64" height="64" />'
    html = html + '<br />' + title + '</a></div>'
    return html


def get_template():
    """ Read template. """
    with open('index.inc', encoding='utf-8') as template_file:
        return template_file.read()


HTML = ''
with open('links.csv', encoding='utf-8') as csvfile:
    linereader = csv.reader(csvfile)
    for row in linereader:
        HTML = HTML + build_entry(row[0], row[1], row[2])

template = get_template()
template = template.replace('@@LINKS@@', HTML)

with open('index.html', 'w', encoding='utf-8') as index_file:
    index_file.write(template)
