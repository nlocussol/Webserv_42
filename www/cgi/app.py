#!/usr/bin/python3

import cgi
import os

form = cgi.FieldStorage()
name = form.getvalue("name")

print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>CGI</title>")
print("</head>")
print("<body>")
print("<h3>Ca c'est mon CGI fils de pute</h3>")
var = os.environ["name"]
print(var)
print("</body>")
print("</html>")
