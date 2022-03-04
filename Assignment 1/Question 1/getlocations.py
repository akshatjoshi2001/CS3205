import requests
import json
ips = []

API_KEY = "0882b2688167fff511f6a936e72e252a"

with open("ping-servers.txt","r") as f:
    ips = f.readlines()
for i in range(len(ips)):
    ips[i] = ips[i].strip()

latlons = []

for ip in ips:
    data = json.loads(requests.get("http://api.ipapi.com/"+ip+ "?access_key="+API_KEY).text)
    lat = str(data["latitude"])
    lon = str(data["longitude"])
    latlons.append(lat+","+lon)
with open("locations.txt","w") as f:
    for latlon in latlons:
        f.write(latlon)
        f.write("\n")

    
