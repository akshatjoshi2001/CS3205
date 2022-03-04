import matplotlib.pyplot as plt
import numpy as np


# Open Log file
logs = []
distances = []
with open("logs.txt","r") as f:
    logs = f.readlines()

# Distances File
with open("distances.txt") as f:
    distances = f.readlines()
for i in range(len(distances)):
    distances[i] = float(distances[i].strip())

rtts = []
for log in logs:
    rtts.append(sum(map(float,log.strip().split(",")[5:]))/10)

rtts2 = np.array(rtts)
distances2 = np.array(distances)

# Fitting straight line to the plot to get speed of transmission from slope

distances2 = np.vstack([distances2,np.ones(rtts2.shape)])
rtts2 = np.reshape(rtts2,(20,1))
distances2 = distances2.T

params = np.linalg.inv((distances2.T)@distances2)@distances2.T@rtts2

speed = 1e3*0.5/params[0][0]
slowfactor = 3e5//speed
print(params)
y1 = [0,distances[1]]
x1 = [params[0][0]*0+params[1][0],params[0][0]*distances[1]+params[1][0]]

print("Speed of transmission: ", str(speed) ," km/s")
print("The speed of transmission is ", str(slowfactor) ,"x slower than light")

plt.figure()
plt.xlabel("Average Round Trip Time(in ms)")
plt.ylabel("Distance (in km)")
plt.scatter(rtts,distances,label="Data Points")

plt.plot(x1,y1,color="r",label="Fitted Line") # fitted line
plt.legend()
plt.savefig("scatter.png")
plt.show()




