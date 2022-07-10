# Based on Fabrício Ferreira Thesis
# https://github.com/MobilityDB/MobilityDB-Deck/blob/main/geojsonvt/data/cors_http_server.py
#!/usr/bin/env python3
# encoding: utf-8
"""Use instead of `python3 -m http.server` when you need CORS"""

from http.server import HTTPServer, SimpleHTTPRequestHandler


class CORSRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET')
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        return super(CORSRequestHandler, self).end_headers()


httpd = HTTPServer(('10.0.2.15', 3333), CORSRequestHandler)
httpd.serve_forever()
