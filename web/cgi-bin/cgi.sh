#!/usr/bin/env bash

content=$(date)
length=${#content}

echo -e "HTTPS/1.1 200 OK\r"
echo -e "Content-Type: text/plain\r"
echo -e "Content-Length: ${length}\r"
echo -e "\r"
echo -e "${content}"
