#!/usr/bin/python3

import cgi
import http.cookies
import os

my_cookie = http.cookies.SimpleCookie()
my_cookie["pref_lang"] = "fr"
my_cookie["pref_lang"]["path"] = "/cgi/"
my_cookie["pref_lang"]["samesite"] = None
my_cookie["pref_lang"]["expires"] = 1000
my_cookie["pref_lang"]["httponly"] = True

print(my_cookie.output())
print("Content-type: text/html; charset=utf-8\r\n\r\n")

html = """<!DOCTYPE html>
<html>
<head>
<title>Mon cookie</title>
<body>
<h1>Example de page avec cookie</h1>
"""

print(html)

try:
    user_lang = http.cookies.SimpleCookie(os.environ["HTTP_COOKIE"])
    print(user_lang["pref_lang"].value)
except (http.cookies.CookieError, KeyError):
    print("Pas de cookie")

html = """</body>
</html>
"""

print(html)
