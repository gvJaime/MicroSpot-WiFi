import urllib.request as urllib2
import time
import math

frequency = 0.15
count = 0

class CircleIterator:
    def __init__(self):
        self.angle = 0
    def __iter__(self):
        return self
    def next(self):
        self.angle = self.angle + 5
        return [0.2*math.cos(self.angle*math.pi/180),0.2*math.sin(self.angle*math.pi/180)]

c = CircleIterator()
timestamp = time.time()
speed = 6/frequency
f = '%.3f' % speed

try:
    print(urllib2.urlopen("http://192.168.4.1/ayy/lmao").read())
    print(urllib2.urlopen("http://192.168.4.1/home").read())
    print(urllib2.urlopen("http://192.168.4.1/move?x=25&y=7.5&f=500").read())
except:
    print("MicroSpot is dead, or disconnected")
    exit()

while True:
    if time.time() - timestamp > frequency:
        timestamp = time.time()
        count = count + 1
        list = c.next()
        x = '%.3f' % list[0]
        y = '%.3f' % list[1]
        try:
            before = time.time()
            print(urllib2.urlopen("http://192.168.4.1/pan?x=" + x + "&y=" + y + "&f=" + f).read())
            after = time.time()
        except Exception as inst:
            print(inst)
            break;
        print(str(count) + " delay: " + str(after - before))

print("MicroSpot died after " + str(count) + " requests, at a rate of " + str(frequency) + " seconds per petition")
