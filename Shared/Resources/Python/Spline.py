from matplotlib import pyplot as plt
from matplotlib.ticker import MultipleLocator

plt.rcParams["figure.figsize"] = [7.50, 7.50]
plt.rcParams["figure.autolayout"] = True
plt.axis('equal')
plt.grid(True)

def ease_overshoot(t: float, s=1.70158):
	if t < 0.5:
		t *= 2.0
		return 0.5 * ((s + 1) * t**3 - s * t**2)
	else:
		t = t * 2.0 - 2.0
		return 0.5 * ((s + 1) * t**3 + s * t**2 + 2)

t_values = [i / 100.0 for i in range(101)]
f_values = [ease_overshoot(t) for t in t_values]

fig, plot = plt.subplots()
plot.plot(t_values, f_values, color='red')
plot.xaxis.set_major_locator(MultipleLocator(0.01))
plot.xaxis.set_minor_locator(MultipleLocator(0.05))
plt.show()