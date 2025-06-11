#!/bin/env bash

IP="localhost"
PORT=3434
DATA="Hello world this is a chunked request"

echo "Testing for port: ${PORT}"
printf "${DATA}" |
	curl "http://${IP}:${PORT}" \
		-X POST \
		-H "Transfer-Encoding: chunked" \
		--trace-ascii output.log \
		--data-binary @-
