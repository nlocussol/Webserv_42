#!/usr/bin/env python3
import os
import http.cookies
import cgi

# Create an instance of SimpleCookie
cookie = http.cookies.SimpleCookie()

# Get existing cookies from the HTTP_COOKIE environment variable
if 'HTTP_COOKIE' in os.environ:
    cookie.load(os.environ['HTTP_COOKIE'])

# Check if a cookie named 'username' exists
if 'username' in cookie:
    # Retrieve the value of the 'username' cookie
    username = cookie['username'].value
    message = f"Welcome back, {username}!"
else:
    # Get the form data
    form = cgi.FieldStorage()

    # Check if the form data contains a 'username' field
    if 'username' in form:
        # Retrieve the value of the 'username' field from the form data
        username = form['username'].value

        # Set the 'username' cookie
        cookie['username'] = username

        # Set the cookie's attributes (optional)
        cookie['username']['expires'] = 365 * 24 * 60 * 60  # 1 year
        cookie['username']['httponly'] = True  # accessible only via HTTP

        message = f"Welcome, {username}!"
    else:
        message = "Please enter a username."

# Send the HTTP header with the Set-Cookie header (if necessary)
if cookie:
    print(cookie.output())

# Send the HTTP header indicating the content type as text/html
print("Content-type: text/html\n")

# Print the HTML response
print("<html>")
print("<head>")
print("<title>Cookie Example</title>")
print("</head>")
print("<body>")
print("<h1>{}</h1>".format(message))
print("<form method='post'>")
print("<label for='username'>Username:</label>")
print("<input type='text' name='username' id='username'>")
print("<input type='submit' value='Submit'>")
print("</form>")
print("</body>")
print("</html>")
