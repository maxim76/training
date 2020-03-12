import os, sys
from http.server import HTTPServer
from http.server import BaseHTTPRequestHandler

class MyRequestHandler(BaseHTTPRequestHandler):
	def do_GET(self):
		print("Request: GET  path %s" % self.path)
		# Endpoint selector
		if self.path == '/hello':
			self.processRequest("hello")
		elif self.path == '/about':
			self.processRequest("about")
		else:
			pass
			# Send page not found

	def processRequest(self, requestType):
		self.send_response(200)
		self.send_header('Content-type','text/html')
		self.end_headers()
		response = "Received request <b>%s</b>" % requestType
		self.wfile.write(bytes(response, "UTF-8"))

webdir = '.' # where your html files and cgi-bin script directory live
port = 80 # default http://localhost/, else use http://localhost:xxxx/
os.chdir(webdir) # run in HTML root dir
srvraddr = ("", port) # my hostname, portnumber
srvrobj = HTTPServer(srvraddr, MyRequestHandler)
srvrobj.serve_forever() # run as perpetual daemon
