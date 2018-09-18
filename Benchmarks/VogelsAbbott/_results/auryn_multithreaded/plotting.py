import matplotlib.pyplot as plt
plt.style.use('ggplot')


SpikeBenchmarkPath = "../timestep_8_delay/Spike.dat"
with open(SpikeBenchmarkPath, 'r') as f:
	SpikeBenchmark = f.read()


AurynBenchmarks = []
AurynBenchmarkCores = [1, 2, 4, 8]

for numCores in AurynBenchmarkCores:
    with open(str(numCores) + ".dat", 'r') as f:
        time = f.read()
	AurynBenchmarks.append(float(time) / 100.0f)




# Plotting single timestep results
fig, ax = plt.subplots()
ax.set_title("Multi-Threaded Vogels Abbot Benchmark Comparison")
ax.scatter(numCores, AurynBenchmarks, color='k')
ax.set_xlabel("Number of CPU Cores")
ax.set_ylabel("Simulation Run Time (Normalized)")
ax.set_ylim([10.0**-2, 10.0**2])
ax.set_yscale('log')
fig.savefig('multithreaded_comparison.png')
#plt.show()
