from http.server import BaseHTTPRequestHandler, HTTPServer
from time import sleep

host = "0.0.0.0"
port = 8010


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        sleep(2)

        self.send_response(200)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.end_headers()
        self.wfile.write(b"Hello from do_GET")

    def do_POST(self):
        content_length = int(self.headers["content-length"])
        print(self.rfile.read(content_length).decode("utf-8"))

        self.do_GET()


with HTTPServer((host, port), Handler) as httpd:
    httpd.serve_forever()
