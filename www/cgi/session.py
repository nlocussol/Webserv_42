#!/usr/bin/python3

import http.cookies
import os
import uuid


def get_uuid_id():
    return str(uuid.uuid4())


cookie_string = os.environ.get("HTTP_COOKIE")
if not cookie_string:
    c = http.cookies.SimpleCookie()
    c["test"] = get_uuid_id()
    c["test"]["expires"] = 5
    print(c.output())

print("Content-type: text/html; charset=utf-8\r\n\r\n")

html = """<!DOCTYPE html>
<html>
<head>
<title>Mon cookie</title>
<body>
<h2>Example de page avec cookie</h2>
"""


print(html)
# print("<font size=+1>Environment</font></br>")
# for param in os.environ.keys():
#     print("<b>%20s</b>: %s</br>" % (param, os.environ[param]))

print("</br>")
try:
    ck = http.cookies.SimpleCookie(os.environ["HTTP_COOKIE"])
    print(ck["test"].value)
except (http.cookies.CookieError, KeyError):
    print("Pas de cookie bozo</br>")
    print("<a href=\"session.py\">Clique ici pour avoir un cookie</a>")

html = """</body>
</html>
"""

print(html)
