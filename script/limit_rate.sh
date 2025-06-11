#!/bin/env bash

IP="localhost"
PORT=3434
DATA="Hello world this is a request request"

echo "Testing for port: ${PORT}"
printf "${DATA}" |
	curl "http://${IP}:${PORT}" \
		-X POST \
		--data-binary @- \
		--trace-ascii output.log \
		--limit-rate "${RATE}"
