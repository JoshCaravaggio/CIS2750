import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy as np

T = np.array([-190, 0, 75, 120])
Energy = np.array([2.7,41.3, 52.5, 34.9])

xnew = np.linspace(T.min(),T.max(),300) 

f = interp1d(T,Energy, kind = 'linear')
energy_smooth = f(xnew)

plt.plot(xnew,energy_smooth)
plt.ylabel("Energy (Joules)")
plt.xlabel("Temperature (Celsius)")
plt.title("6061-T6 Aluminum")
plt.show()