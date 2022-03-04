import subprocess
#Note: Works only on Linux.
def gettime(line):
    # For Linux
    if(len(line) == 0 ): # Request timed out
        print("Request Time Out detected. Check your network connection and rerun the script.")
        exit()
        return "inf"
    return line.split("=")[3].split(" ")[0]
    
def getstr(*args):
    result =  ""
    for arg in args:
        result += arg + ',' #Inefficient. But Works. 
    return result[:-1]
ips = []
rtts = []
with open("ping-servers.txt","r") as f:
    ips = f.readlines()
for i in range(len(ips)):
    ips[i] = ips[i].strip()


for ip in ips:
    try:
        print("Calculating rtt for ip ",ip)
        process = subprocess.Popen(["ping","-c 10",ip],stdout=subprocess.PIPE,universal_newlines=True)
        stdout,stderr = process.communicate()
        lines = stdout.split("\n")
        rttsforip = list(map(gettime,lines[1:11]))
        rtts.append(rttsforip)
    except:
        print("Error in getting rtt for ip ",ip, " . Either the server is down or there is an issue with your network.")
        print("Try rerunning the script ones network/dest server is back up.")
        exit()

mylat = 12.99
mylon = 80.23

raw_locations = []
with open("locations.txt","r") as f:
    raw_locations = f.readlines()

locations = []
for loc in raw_locations:
    locations.append(tuple(loc.strip().split(",")))




with open("logs.txt","w") as f:
    for i in range(len(ips)):
    
        f.write(getstr(str(mylat), str(mylon), ips[i], *locations[i],*rtts[i]))
        f.write("\n")


    





    