import numpy as np
import scipy.optimize as sp
import scipy.stats as stat
import matplotlib.pyplot as plt

x_vals = [8.4e-16, 1.2e-10, 1.7e-10, 1.1e-6, 3.8e-6, 1.6e1, 2.0e1, 1.7e6, 6.4e6, 7.0e8, 1.4e14, 5.0e20]
y_vals = [1.6e-27, 1.7e-27, 2.0e-26, 1.0e-15, 2.7e-14, 6.2e1, 1.8e5, 7.3e22, 5.9e24, 1.9e30, 2.0e30, 2.9e42]

log_x_vals = np.log(x_vals)
log_y_vals = np.log(y_vals)

def objective(x,a,b):
    return a * x + b

slope1, intercept1, r1, p1, se1 = stat.linregress(x_vals, y_vals)
slope2, intercept2, r2, p2, se2 = stat.linregress(log_x_vals, log_y_vals)

a1,b1 = (slope1,intercept1)
a2,b2 = (slope2,intercept2)
new_ys = []
new_log_ys = []
for (x,logx) in zip(x_vals,log_x_vals):
    new_ys.append(objective(x,a1,b1))
    new_log_ys.append(objective(logx,a2,b2))


fig1 = plt.figure()
fig2 = plt.figure()

ax_1 = fig1.add_subplot(111)
ax_2 = fig2.add_subplot(111)

ax_1.set_title('Size vs Mass (linear)')
ax_2.set_title('Size vs Mass (log-log)')

ax_1.scatter(x_vals, y_vals)
ax_1.plot(x_vals, new_ys)

ax_2.scatter(log_x_vals, log_y_vals)
ax_2.plot(log_x_vals, new_log_ys)

ax_1.set_xlabel('Size (m)')
ax_2.set_xlabel('Size (m)')
ax_1.set_ylabel('Mass (kg)')
ax_2.set_ylabel('Mass (kg)')

ax_1.set_xticklabels([0,5.0e19,1.0e20,2.0e20,3.0e20,4.0e20,5.0e20], fontsize=10)
ax_1.set_yticklabels([1e-20,5.0e41,1.0e42,1.5e42,2.0e42,2.5e42,3.0e42], fontsize=8)


ax_1.text(0,2.5e42, 'y={:0.2e}x+{:0.2e}, r={:0.2f}'.format(a1,b1,r1))
ax_2.text(-25,100, 'y={:0.2e}x+{:0.2e}, r={:0.2f}'.format(a2,b2,r2))

fig1.savefig("./PHYS164a/linear.png")
fig2.savefig("./PHYS164a/loglog.png")

# plt.show()
