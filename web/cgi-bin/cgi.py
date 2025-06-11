#!/usr/bin/env python

import sys
from datetime import datetime

content: str = str(datetime.now())  # might format the output
length: int = len(content)

sys.stdout.write("HTTPS/1.1 200 OK\r\n")
sys.stdout.write("Content-Type: text/plain\r\n")
sys.stdout.write(f"Content-Length: {length}\r\n")
sys.stdout.write("\r\n")
sys.stdout.write(content)
