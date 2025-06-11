#!/bin/bash

# En-tête HTTP
echo -e "Content-Type: text/html\r\n\r\n"

# Récupération des informations système
OS_VERSION=$(uname -a)
UPTIME=$(uptime -p)
DISK_USAGE=$(df -h / | awk 'NR==2 {print $3 " of " $2 " used (" $5 ")}')
MEMORY_USAGE=$(free -h | awk '/Mem/ {print $3 " of " $2 " used (" $3/$2*100 "%)"}')
CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | sed "s/.*, *\([0-9.]*\)%* id.*/\1/" | awk '{print 100 - $1 "%"}')
PROCESS_COUNT=$(ps aux --no-headers | wc -l)
NETWORK_INFO=$(ifconfig -a)
DISK_INFO=$(lsblk)
CPU_INFO=$(lscpu)
SYSTEM_TIME=$(date)
TIMEZONE=$(timedatectl show --property=Timezone --value)
IP_INFO=$(hostname -I)

# Création de la page HTML avec un beau CSS
echo -e "<html><head><title>System Info</title>"
echo -e "<style>"
echo -e "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f0f0f5; color: #333; margin: 0; padding: 0; }"
echo -e "h1 { text-align: center; color: #4CAF50; margin-top: 20px; }"
echo -e ".container { max-width: 900px; margin: 40px auto; padding: 20px; background: white; border-radius: 8px; box-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1); }"
echo -e "table { width: 100%; border-collapse: collapse; margin-top: 20px; }"
echo -e "table th, table td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
echo -e "table th { background-color: #4CAF50; color: white; }"
echo -e "table tr:nth-child(even) { background-color: #f2f2f2; }"
echo -e "p { font-size: 18px; color: #555; line-height: 1.6; }"
echo -e "strong { color: #333; font-weight: bold; }"
echo -e "#back-btn { position: absolute; top: 20px; left: 20px; padding: 10px 20px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; }"
echo -e "#back-btn:hover { background-color: #45a049; }"
echo -e "</style>"
echo -e "</head><body>"

# Ajout du bouton retour en haut à gauche
echo -e "<button id=\"back-btn\" onclick=\"window.history.back();\">Back</button>"

echo -e "<div class=\"container\">"
echo -e "<h1>System Information</h1>"

# Affichage des informations système
echo -e "<table>"
echo -e "<tr><th>Property</th><th>Value</th></tr>"
echo -e "<tr><td><strong>OS Version</strong></td><td>$OS_VERSION</td></tr>"
echo -e "<tr><td><strong>Uptime</strong></td><td>$UPTIME</td></tr>"
echo -e "<tr><td><strong>Disk Space Usage</strong></td><td>$DISK_USAGE</td></tr>"
echo -e "<tr><td><strong>Memory Usage</strong></td><td>$MEMORY_USAGE</td></tr>"
echo -e "<tr><td><strong>CPU Usage</strong></td><td>$CPU_USAGE</td></tr>"
echo -e "<tr><td><strong>Running Processes</strong></td><td>$PROCESS_COUNT processes</td></tr>"
echo -e "<tr><td><strong>IP Address</strong></td><td>$IP_INFO</td></tr>"
echo -e "<tr><td><strong>System Time</strong></td><td>$SYSTEM_TIME</td></tr>"
echo -e "<tr><td><strong>Timezone</strong></td><td>$TIMEZONE</td></tr>"
echo -e "<tr><td><strong>Disk Info</strong></td><td><pre>$DISK_INFO</pre></td></tr>"
echo -e "<tr><td><strong>Network Info</strong></td><td><pre>$NETWORK_INFO</pre></td></tr>"
echo -e "</table>"

echo -e "<p><a href=\"#\" onclick=\"window.location.reload();\">Refresh</a></p>"

echo -e "</div>"

echo -e "</body></html>"
