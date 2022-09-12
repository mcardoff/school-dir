import numpy as np
import matplotlib.pyplot as plt

filename = "SiPlot"

counts = [10459, 9617, 3738]
ismm = True
dvals = [0.0, 4.25, 80.65]
d = np.array([ x / 10 for x in dvals]) if ismm else dvals
normcounts = np.array([ np.log(x / max(counts)) for x in counts])

fig = plt.figure()

plt.rc('font', size=15)          # controls default text sizes
plt.rc('axes', titlesize=15)     # fontsize of the axes title
plt.rc('axes', labelsize=15)     # fontsize of the x and y labels
plt.rc('xtick', labelsize=10)    # fontsize of the tick labels
plt.rc('ytick', labelsize=10)    # fontsize of the tick labels
plt.rc('legend', fontsize=15)    # legend fontsize
plt.rc('figure', titlesize=20)   # fontsize of the figure title

ax1 = fig.add_subplot(111)
ax1.set_ylabel("ln(N) (counts)")
ax1.set_xlabel("d (cm)")
ax1.set_title("ln(I/I_0) vs d")

plt.scatter(d, normcounts, color='red')

linear_model = np.polyfit(d, normcounts, 1)
linear_model_fn = np.poly1d(linear_model)
plt.plot(d,linear_model_fn(d))

plt.text(max(d)-4, max(normcounts)-0.1,
         'ln(N/N_0)={0[0]:.3g}d+{0[1]:.3g}'.format(linear_model), fontsize = 12)

plt.savefig("./figs/" + filename + ".png")

plt.show()
