import matplotlib.pyplot as plt
import numpy as np
#plt.style.use('ggplot')
plt.style.use('seaborn-paper')

simulators = [
        "Spike",
        "GeNN",
        "Auryn",
        "Brian2",
        "ANNarchy",
        "NEST",
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
#ax.set_title("Comparison Simulator with the Brunel Benchmark", size=12)
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
ax.set_xticklabels(simulators, fontsize=12, rotation=-45)
#ax.set_xlabel("Simulator", size=12)
ax.set_ylabel("Simulation Run Time (Normalized)", size=12)
ax.yaxis.set_tick_params(labelsize=11)
ax.set_yscale('log')
ax.set_ylim([10.0**-2, 10.0**4])
ax.legend()
fig.subplots_adjust(bottom=0.175, top=0.975)
fig.savefig('Brunel_Comparison.png', dpi=300)
#plt.show()
