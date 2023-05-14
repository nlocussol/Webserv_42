#!/usr/bin/ruby

require 'cgi'
require 'cgi/cookie'

cgi = CGI.new("html4")
cookie = CGI::Cookie.new('name' => 'test', 'value' => 'fdp', 'expires' => Time.now + 3600)
cgi.out('cookie' => cookie) do
  chi.head + cgi.body { "Cookie stored"}
end
