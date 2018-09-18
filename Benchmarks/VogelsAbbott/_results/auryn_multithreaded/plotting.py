import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit
plt.style.use('ggplot')


SpikeBenchmarkPath = "../timestep_8_delay/Spike.dat"
with open(SpikeBenchmarkPath, 'r') as f:
        SpikeBenchmark = f.read()
SpikeBenchmark = float(SpikeBenchmark) / 100.0


AurynBenchmarks = []
AurynBenchmarkCores = [1, 2, 4, 8]

for numCores in AurynBenchmarkCores:
    with open(str(numCores) + ".dat", 'r') as f:
        time = f.read()
        AurynBenchmarks.append(float(time) / 100.0)

print(AurynBenchmarkCores, AurynBenchmarks)

# Fitting a line
points = np.linspace(0, 8, 100)

def expfunc(x, a, b, c):
    return a * np.exp(-b *x) + c

popt, pcov = curve_fit(expfunc, AurynBenchmarkCores, AurynBenchmarks)
#plt.plot(points, expfunc(points, *popt))



# Plotting single timestep results
fig, ax = plt.subplots()
ax.set_title("Multi-Threaded Vogels Abbot Benchmark Comparison")
ax.scatter(AurynBenchmarkCores, AurynBenchmarks, color='k', zorder=2, s=50)
ax.plot([0, 8], [SpikeBenchmark, SpikeBenchmark], color='r', linewidth = 5, alpha=0.5, zorder=1, label="Spike", linestyle='--')
ax.plot(points, expfunc(points, *popt), color='b', linewidth = 5, alpha=0.5, zorder=1, label="auryn")
ax.legend()
ax.set_xlabel("Number of CPU Cores")
ax.set_ylabel("Simulation Run Time (Normalized)")
ax.set_ylim([10.0**-2, 10.0**0])
ax.set_yscale('log')
fig.savefig('multithreaded_comparison.png')
#plt.show()
