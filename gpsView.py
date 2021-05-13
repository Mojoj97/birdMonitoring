import mysql.connector
#import MySQLdb as dbapi
import sys
import csv
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import urllib.request
import shutil
import os
from bs4 import BeautifulSoup
from dataclasses import dataclass


mydb = mysql.connector.connect(
  host="192.168.0.213",
  user="bird",
  database="esp_data",
  password="cemmacemma"
)

print(mydb)

cursor = mydb.cursor()
query = ("SELECT * FROM SensorData ")
cursor.execute(query)

result = cursor.fetchall()
print(result)
c = csv.writer(open('birdMonitor.csv', 'w', encoding='utf-8'))
for x in result:
    c.writerow(x)
    print(c.writerow(x))

results = []
with open("birdMonitor.csv") as csvfile:
    reader = csv.reader(csvfile) # change contents to floats
    for row in reader: # each row is a list
        results.append(row)

 #print(results)

sensorDatas = []
latitudes = []
longitudes = []
ids = []
temperatures = []
humidities = []
ringnumbers = []
dates = []


for i in range(len(results)):
  ids.append(results[i][0])
  ringnumbers.append(results[i][1])
  latitudes.append(results[i][2])
  longitudes.append(results[i][3])
  temperatures.append(results[i][4])
  humidities.append(results[i][5])
  dates.append(results[i][6])

m = Basemap(projection='mill',
           llcrnrlat = -90,
           urcrnrlat = 90,
           llcrnrlon = -180,
           urcrnrlon = 180,
           resolution = 'c')
#m.bluemarble(scale=0.5)
m.drawcoastlines()
#m.etopo(scale=0.5, alpha=0.5)
m.drawparallels(np.arange(-90,90,10),labels=[True,False,False,False])
m.drawmeridians(np.arange(-180,180,20),labels=[0,0,0,1])

for i in range(len(ringnumbers)-1):
    x,y = m(float(longitudes[i]), float(latitudes[i]))
    tmp_str = ids[i] + " | " + temperatures[i] + "°C " + humidities[i] + "%"
    plt.scatter(x,y, zorder=1)
    plt.plot(x,y, zorder = 2)
    plt.text(x,y, tmp_str, fontsize=10)        

plt.title('Bird Monitoring', fontsize=20)

plt.show()

