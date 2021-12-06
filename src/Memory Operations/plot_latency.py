import matplotlib.pyplot as plt
import numpy as np
import json

f = open('latency_result.json')

data = json.load(f)

vecSize = data['size_byte']

vecLatency = data['latency_ns']

print(vecSize)
print("latency:")
print(vecLatency)

# i = 0
# while i < len(vecSize):
#    size = vecSize[i]
#    latency = vecLatency[i]
#    plt.scatter(size, latency)
#    i += 1

x = np.array(vecSize)
y = np.array(vecLatency)
plt.plot(x,y)

plt.xlabel('size (byte)')
plt.ylabel('latency (nanosecond)')
plt.xscale('log')
plt.yscale('log')
plt.show()