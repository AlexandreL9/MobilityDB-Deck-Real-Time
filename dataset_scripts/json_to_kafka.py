from confluent_kafka import Producer, Consumer, KafkaError
import json
import sys
import time
from datetime import datetime, timedelta

p = Producer({'bootstrap.servers': 'localhost:9092','broker.version.fallback': '0.9.0.0', 'api.version.request': False})

f = open(sys.argv[1])
data = json.load(f)

TRAJECTORY_SIZE = 120 # one position per second
KAFKA_TOPIC = "positions"
startingtimestamp = datetime.now()

for t in range(0, TRAJECTORY_SIZE):
    routeCounter=0
    for routeID in range(len(data)):
        try:
            value = str(data[routeID]["vehicleID"])+","+str(data[routeID]["path"][t][0])+","+str(data[routeID]["path"][t][1])+","+str((startingtimestamp+timedelta(seconds=t)).strftime("%Y-%m-%d %H:%M:%S"))
            p.produce(KAFKA_TOPIC, value=value.encode('utf-8'))
            p.poll(0)
            routeCounter+=1
        except Exception as e:
            print("Produce Error : "+str(e))
    time.sleep(1)
    print("At instant "+str(t)+" produced "+str(routeCounter)+" / "+str(len(data))+" routes positions")

p.flush(0)
