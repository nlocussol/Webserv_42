#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Usage: ./tester.sh port"
	echo "where port is the port from the conf file"
	exit
fi

SERVER_URL="http://localhost"
POST_DATA='{"title": "New Post", "content": "Lorem ipsum dolor sit amet."}'
DATA='Lorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris ut maximus mauris. Suspendisse ac accumsan urna. Maecenas risus tellus, varius in pretium et, convallis eu lacus. Vivamus faucibus eros at dignissim viverra. Etiam vitae augue lacus. Vivamus sagittis lacus quis eros euismod, et ultricies felis tristique. Maecenas ullamcorper pharetra tellus, quis eleifend urna cursus id. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Aliquam risus felis, viverra sed tellus quis, facilisis convallis ante. Morbi aliquam quam in cursus maximus. Fusce id massa rhoncus eros tincidunt consectetur ut sed libero. Duis at ipsum lacinia metus commodo rhoncus ac quis ipsum.'

echo "Tester webserv 42"
echo "on $SERVER_URL:$1"

#telnet localhost $1
echo "Siege on for 3seconds"
siege 127.0.0.1:$1 -b &
sleep 3
pkill siege

echo "Classic get request"
curl -d @index.html -v $SERVER_URL:$1
echo "Classic post request"
curl -X POST -d @index.html -v $SERVER_URL:$1
echo -n "post request which upload a file"
curl -X POST -F "file=@index.html" $SERVER_URL:$1
echo "Classic del request"
curl -X DELETE $SERVER_URL/index.html
echo "post request with content"
curl -X POST -v $SERVER_URL:$1 -d "$POST_DATA"
echo "post request with chunked parameter"
echo -n "$DATA" | curl -i -X POST $SERVER_URL:$1 -H "Transfer-Encoding: chunked" --data-binary @-
