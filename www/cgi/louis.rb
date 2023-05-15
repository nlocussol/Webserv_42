#!/usr/bin/ruby

require 'cgi'

cgi = CGI.new('html4')

puts "Content-Type: text/html\r\n\r\n"
puts "<html><body><h1>Welcome to Ruby CGI</h1>"
puts "<h3>Hello "
puts cgi['name']
puts "</br>"
puts "</h1></body></html>"
