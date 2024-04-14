#!/usr/bin/env python3
""" eplogr

Add to profile:
    alias eplogr="python3 $HOME/sys/eplogr.py"

Requirement: pyngrok
"""
# pylint: disable=too-few-public-methods
# pylint: disable=locally-disabled
# pylint: disable=duplicate-code
# pylint: disable=bare-except
# pylint: disable=too-many-instance-attributes
import os
import platform
import sys
import urllib
from http.server import BaseHTTPRequestHandler, HTTPServer
# pylint: disable=E0401
from pyngrok import conf, ngrok
# pylint: enable=E0401


class MyServer(BaseHTTPRequestHandler):
    """ My Web server :-) """

    #pylint: disable=C0103
    def do_POST(self):
        """ Handle POST request. """
        content_len = int(self.headers.get('Content-Length'))
        post_body = self.rfile.read(content_len)
        self.send_response(200)
        self.send_header("Content-type", "application/json")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        print(post_body.decode('utf-8'))
    #pylint: enable=C0103


class Eplogr():
    """ Eplogr backbone. """

    def execute(self):
        """ Events backbone handler. """
        home = os.path.expanduser('~')
        os_type = platform.system()
        if os_type == 'Darwin':
            xdg_location = home + '/Library/Application Support/ngrok/ngrok.yml'
        else:
            xdg_location = home + '/.config/ngrok/ngrok.yml'

        print(f"Running on {os_type}, relocating config to {xdg_location}")
        conf.get_default().config_path = xdg_location

        tunnel = self.get_tunnel()
        print(f"Starting tunnel: {tunnel.public_url}")

        if not "addr" in tunnel.config:
            print("Cannot find remote endpoint in ngrok.yml")
            sys.exit(1)

        parsed_url = urllib.parse.urlparse(tunnel.config["addr"])

        print(f"Starting local webserver on {parsed_url.netloc}")
        web_server = HTTPServer((parsed_url.hostname, parsed_url.port), MyServer)
        try:
            web_server.serve_forever()
        except KeyboardInterrupt:
            pass

        web_server.server_close()


    def get_tunnel(self):
        """ Open ngrok tunnel """
        if len(sys.argv) > 1:
            return ngrok.connect(name=sys.argv[1])

        return ngrok.connect(name="eplogr")


if __name__ == '__main__':
    Eplogr().execute()
