import matplotlib.pyplot as plt
import numpy as np
plt.style.use('ggplot')

simulators = [
        "Spike",
        "auryn",
        "ANNarchy",
        "brian2",
        "Nest",
        ]


non_plastic_speedresults = []
non_plastic_foldername = "simulation_speed/non_plastic/"
plastic_speedresults = []
plastic_foldername = "simulation_speed/plastic/"

for s in simulators:
    with open(non_plastic_foldername+s+".dat", 'r') as f:
        time = f.read()
        non_plastic_speedresults.append(float(time) / 100.0)
    with open(plastic_foldername+s+".dat", 'r') as f:
        time = f.read()
        plastic_speedresults.append(float(time) / 100.0)




# Plotting single timestep results
fig, ax = plt.subplots()
ax.set_title("Brunel 10^7 Synapse Benchmark Result")
#tick_label=simulators,
barwidth = 0.4
ax.bar(
        np.arange(len(simulators)) - (barwidth / 2.0),
        non_plastic_speedresults,
        width=barwidth,
        align='center', color='k', label="Static Weights")
ax.bar(
        np.arange(len(simulators)) + (barwidth / 2.0),
        plastic_speedresults,
        width=barwidth,
        align='center', color='r', label="Plasticity ON")

ax.set_xticks(np.arange(len(simulators)))
ax.set_xticklabels(simulators)
ax.set_xlabel("Simulator")
ax.set_ylabel("Simulation Run Time (Normalized)")
ax.set_yscale('log')
ax.set_ylim([10.0**-2, 10.0**4])
ax.legend()
fig.savefig('speed_comparison.png')
#plt.show()
