#!/bin/env bash

IP="localhost"
PORT=3434
SERVER_NAMES=(
	"NULL"
	"spider_web"
	"spider_web:1414"
	"spider_web:3434"
	"test"
)

echo "Testing for port: ${PORT}\n"
for server_name in "${SERVER_NAMES[@]}"; do
	echo "Testing for server_name: ${server_name}"
	curl "http://${IP}:${PORT}" \
		-X GET \
		-H "Host: ${server_name}" \
		-s \
		--trace-ascii output.log
	echo -en "\n\n"
done
