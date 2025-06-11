#!/usr/bin/env python3

import datetime

print("Content-Type: text/html\r")
print("\r")
print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Live Clock - Python CGI</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            position: relative;
        }}
        .container {{
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px rgba(0,0,0,0.1);
            text-align: center;
        }}
        h1 {{
            color: #007BFF;
            margin-bottom: 10px;
        }}
        .clock {{
            font-size: 2rem;
            font-weight: bold;
            color: #333;
        }}
        #back-btn {{
            position: absolute;
            top: 20px;
            left: 20px;
            padding: 10px 20px;
            background-color: #007BFF;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
        }}
        #back-btn:hover {{
            background-color: #0056b3;
        }}
    </style>
    <script>
        function updateClock() {{
            let now = new Date();
            let hours = now.getHours().toString().padStart(2, '0');
            let minutes = now.getMinutes().toString().padStart(2, '0');
            let seconds = now.getSeconds().toString().padStart(2, '0');
            document.getElementById("time").innerHTML = hours + ":" + minutes + ":" + seconds;
        }}
        setInterval(updateClock, 1000);
        window.onload = updateClock;
    </script>
</head>
<body>
    <button id="back-btn" onclick="window.history.back();">Back</button>
    <div class="container">
        <h1>Live Clock</h1>
        <p class="clock"><span id="time">{datetime.datetime.now().strftime("%H:%M:%S")}</span></p>
    </div>
</body>
</html>
""")
