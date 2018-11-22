from matplotlib import pyplot as plt
xValues = []
yValues = []
for x in range(-10, 11):
	xValues.append(x)
	yValues.append(x**2)
plt.plot( xValues, yValues, 'g-' )
plt.show()


# Another approach with numpy array
# all sequences passed to pyplotlib are converted to numpy arrays internally.
import numpy as np
x = np.linspace(-2, 2, 20)
plt.plot( x, x, 'b-', label='linear' )
plt.plot( x, x**2, 'g-', label='quadratic' )
plt.plot( x, x**3, 'r-', label='cubic' )
plt.legend()
plt.grid(True)
plt.show()


fig = plt.figure()

plt.subplot(2,2,1)
x = np.linspace(0, np.pi*4, 100)
y = np.sin(x)
plt.grid(True)
plt.plot(x,y)

plt.subplot(2,2,2)
x = np.linspace(0, np.pi*4, 100)
y = np.cos(x)
plt.grid(True)
plt.plot(x,y)

plt.subplot(2,2,3)
x = np.linspace(0, np.pi*4, 500)
y = np.exp(x)
plt.grid(True)
plt.plot(x,y)

plt.subplot(2,2,4)
x = np.linspace(0, np.pi*4, 500)
y = np.log(x)
plt.grid(True)
plt.plot(x,y)

plt.show()
# Try to create image
