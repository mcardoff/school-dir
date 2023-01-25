import numpy as np
import scipy.optimize as sp
import matplotlib.pyplot as plt

x_vals = np.linspace(0,10,100)
y_vals = [x ** 2 for x in x_vals]

def linear(t,a,b):
    return (t * a) + b

par, cov = sp.curve_fit(linear,x_vals,y_vals)
print(par)
plt.plot(x_vals,y_vals, '*')
plt.show()
