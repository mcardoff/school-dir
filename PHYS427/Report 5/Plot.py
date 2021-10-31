import numpy as np
import matplotlib.pyplot as plt

d = np.array([ x / 10 for x in [0.0, 1.0, 2.1, 3.8, 4.7]])
counts = np.array(
    [ np.log(x / 72052) for x in [72052, 64517, 56976, 46573, 41182]])

fig = plt.figure()

ax1 = fig.add_subplot(111)
ax1.set_ylabel("N")
ax1.set_xlabel("d (cm)")
ax1.set_title("ln(I/I_0) vs d")

plt.scatter(d, counts)
plt.show()
