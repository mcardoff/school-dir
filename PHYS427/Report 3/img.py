import numpy as np
import matplotlib.pyplot as plt

def gaussian(x):
    return np.exp(-(x ** 2))

x = np.linspace(-3,3,100)
gy = gaussian(x)
line1x = np.array([np.sqrt(np.log(2))]*100)
line2x = np.array([-np.sqrt(np.log(2))]*100)
line3x = np.array([0]*100)
liney  = np.linspace(0,0.55,100)
line3y = np.linspace(0,1.05,100)

fig, ax = plt.subplots()

plt.xlabel('Pulse Height')
plt.ylabel('Count')
plt.rcParams["axes.labelsize"] = 22
plt.text(1, 0.5, "FWHM", fontdict=None)
plt.text(0, 0, "Centroid", fontdict=None)

ax.set_xticklabels('')
ax.set_yticklabels('')

plt.plot(x,y,line1x, liney, 'r' , line2x, liney, 'r', line3x, line3y, 'r')
plt.savefig("./img/fwhmdemo.png")
plt.show()
