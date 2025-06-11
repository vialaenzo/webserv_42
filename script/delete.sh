#!/bin/env bash

IP="localhost"
PORT=3434

echo "Testing for port: ${PORT}"
curl "http://${IP}:${PORT}" \
	-X POST \
	-H "Content-Type: application/json" \
	-d '{"key1":"value1", "key2":"value2"}'
