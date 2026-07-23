import sys
import numpy as np
import matplotlib.pyplot as plt



filename = (
    sys.argv[1]
    if len(sys.argv)>1
    else "power.bin"
)



# uint32 binary power data
power = np.fromfile(
    filename,
    dtype=np.uint32
)


window = 200000

power = power[:window]

plt.figure(figsize=(12,4))

plt.plot(
    power,
    linewidth=0.8
)


plt.title(
    "ADS-B Power Envelope"
)


plt.xlabel(
    "Sample Index"
)

plt.ylabel(
    "Power I² + Q²"
)

plt.grid(True)

plt.show()
