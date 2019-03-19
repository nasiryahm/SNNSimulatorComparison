import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit
#plt.style.use('ggplot')
plt.style.use('seaborn-paper')

simulators = [
    "Auryn",
    "Spike"
]

scales = [
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256
]

speeds = [[] for s in simulators]


for sindex in range(len(simulators)):
    path = "./" + simulators[sindex] + "/"
    for cindex in range(len(scales)):
        subpath = path + str(scales[cindex]) + ".dat"
        time = "0.0"
        try:
            with open(subpath, 'r') as f:
                time = f.read() 
        except:
            pass
        time = float(time)
        speeds[sindex].append(time)


normalizedspeeds = [[] for s in simulators]
for sindex in range(len(simulators)):
    for cindex in range(len(scales)):
        normalizedspeeds[sindex].append(speeds[sindex][cindex] / speeds[sindex][0])


# Plotting single timestep results
fig, ax = plt.subplots()
#ax.set_title("Multi-Threaded Vogels Abbott Benchmark Comparison\nauryn vs Spike", size=12)
ax.scatter(scales, normalizedspeeds[0], color='k', zorder=5, s=25, label=simulators[0])
ax.scatter(scales, normalizedspeeds[1], color='r', zorder=2, s=25, label=simulators[1])
plt.plot([-25,300],[-25,300], color='k', alpha=0.5, zorder=0, linewidth=1, linestyle='--')
#ax.plot([1, 8], [SpikeBenchmark, SpikeBenchmark], color='r', linewidth = 3, alpha=0.75, zorder=1, label="Spike", linestyle='--')
#ax.plot(points, expfunc(points, *popt), color='k', linewidth = 3, alpha=0.75, zorder=1, label="Auryn")
ax.legend(loc=2, frameon=False)
ax.yaxis.set_tick_params(labelsize=11)
ax.xaxis.set_tick_params(labelsize=11)
ax.set_xlabel("Relative Network Scale", size=12)
ax.set_ylabel("Relative Simulation Run Time", size=12)
newscale = [1,32,64,128,256]
ax.set_xticks(newscale)
#ax.set_ylim([10.0**-2, 10.0**0])
#ax.set_yscale('log')
fig.subplots_adjust(bottom=0.15, top=0.975)
fig.savefig('netscaling_comparison.png', dpi=300)
#plt.show()

print(speeds)
exit()

# Fitting a line
points = np.linspace(1, 8, 100)

def expfunc(x, a, b, c):
    return a * np.exp(-b *x) + c

popt, pcov = curve_fit(expfunc, AurynBenchmarkCores, AurynBenchmarks)
#plt.plot(points, expfunc(points, *popt))


