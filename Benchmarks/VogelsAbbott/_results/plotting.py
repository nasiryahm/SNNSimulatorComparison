import matplotlib.pyplot as plt
plt.style.use('ggplot')

simulators = [
        "Spike",
        "auryn",
        "brian2genn",
        "ANNarchy",
        "brian2",
        "Nest",
        ]

single_timestep_delay_results = []
singlefoldername = "timestep_single_delay/"

eight_timestep_delay_results = []
eightfoldername = "timestep_8_delay/"

for s in simulators:
    with open(singlefoldername+s+".dat", 'r') as f:
        time = f.read()
        single_timestep_delay_results.append(float(time) / 100.0)
    with open(eightfoldername+s+".dat", 'r') as f:
        time = f.read()
        eight_timestep_delay_results.append(float(time) / 100.0)




# Plotting single timestep results
fig, ax = plt.subplots()
ax.set_title("Vogels-Abbott Benchmark Result with 0.1ms synaptic delays")
ax.bar(
        range(len(simulators)),
        single_timestep_delay_results,
        tick_label=simulators,
        align='center', color='k')
ax.set_xlabel("Simulator")
ax.set_ylabel("Simulation Speed (Normalized)")
ax.set_yscale('log')
fig.savefig('single_delay_comparison.png')
#plt.show()


fig, ax = plt.subplots()
ax.set_title("Vogels-Abbott Benchmark Result with 0.8ms synaptic delays")
ax.bar(
        range(len(simulators)),
        eight_timestep_delay_results,
        tick_label=simulators,
        align='center', color='k')
ax.set_xlabel("Simulator")
ax.set_ylabel("Simulation Speed (Normalized)")
ax.set_yscale('log')
fig.savefig('eight_delay_comparison.png')
#plt.show()
