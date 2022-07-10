import json
import time
import sys
from datetime import datetime, timedelta

f = open(sys.argv[1])

data = json.load(f)

startingtimestamp = datetime.now()
resTrajectories = []
TRAJECTORY_SIZE = 120 # one position per second
vehicleID = 0

for route in data:
    resTrajectory = route["path"]
    resTimestamps = []
    while len(resTrajectory) < TRAJECTORY_SIZE:
        if route["path"][0] != route["path"][-1]:
            for p in range(len(route["path"])-1,-1,-1):
                resTrajectory.append(route["path"][p])
        else:
            for p in range(len(route["path"])):
                resTrajectory.append(route["path"][p])
    
    for t in range(len(resTrajectory)):
        resTimestamps.append((startingtimestamp+timedelta(seconds=t)).strftime("%Y-%m-%d %H:%M:%S")) # en seconde
        
    resTrajectories.append({"vehicleID" : vehicleID, "path" : resTrajectory, "timestamps" : resTimestamps})
    vehicleID +=1
    
    
with open('trips_extended.json', 'w') as f:
    json.dump(resTrajectories, f, indent=2)