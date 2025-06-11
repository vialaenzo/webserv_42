#!/bin/env bash

IP="localhost"
PORT=3434

echo "Testing for port: ${PORT}"
curl "http://${IP}:${PORT}" \
	-X PUT
