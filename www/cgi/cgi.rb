require 'cgi'
require 'cgi/session'

cgi = CGI.new
session = CGI::Session.new(cgi)

# Check if the 'my_cookie' cookie is already set in the session
if session['my_cookie']
  my_cookie_value = session['my_cookie']
else
  # Set a default value if the cookie is not set
  my_cookie_value = 'Default Value'
  session['my_cookie'] = my_cookie_value
  session.close
end

# Check if the form has been submitted
if cgi.request_method == 'POST'
  # Retrieve the new value from the submitted form data
  new_value = cgi.params['new_value'].first

  # Update the value of the cookie in the session
  session['my_cookie'] = new_value
  session.close

  # Add the cookie to the response
  cgi.out('cookie' => session.cookie) do
    # Display a success message
    "Cookie has been set to: #{new_value}"
  end
else
  # Display the current value of the cookie and a form to update it
  cgi.out do
    <<~HTML
      <html>
        <head><title>Cookie Handling</title></head>
        <body>
          <p>Current value of 'my_cookie': #{my_cookie_value}</p>
          <form method="post" action="">
            <label for="new_value">New Value:</label>
            <input type="text" name="new_value" id="new_value" />
            <input type="submit" value="Update Cookie" />
          </form>
        </body>
      </html>
    HTML
  end
end
