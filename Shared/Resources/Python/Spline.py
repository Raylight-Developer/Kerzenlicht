import numpy as np
from matplotlib import pyplot as plt

plt.rcParams["figure.figsize"] = [7.50, 7.50]
plt.rcParams["figure.autolayout"] = True
plt.axis('equal')
plt.grid(True)
plt.xticks([i / 20.0 for i in range(21)])
plt.yticks([i / 20.0 for i in range(21)])

def ease_overshoot(t: float, s=1.70158):
	if t < 0.5:
		t *= 2.0
		return 0.5 * ((s + 1) * t**3 - s * t**2)
	else:
		t = t * 2.0 - 2.0
		return 0.5 * ((s + 1) * t**3 + s * t**2 + 2)

t_values = [i / 100.0 for i in range(101)]
f_values = [ease_overshoot(t) for t in t_values]

plt.plot(t_values, f_values, color='red')
plt.show()