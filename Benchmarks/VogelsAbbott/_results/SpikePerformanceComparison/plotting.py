import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 40})
plt.style.use('seaborn-paper')

comparisons = [
        "NONE",
        "ASG_ONLY",
        "TG_ONLY",
        "TG_ASG",
        ]

benchmarks = []

for names in comparisons:
    with open(names+".dat", 'r') as f:
        time = f.read()
        benchmarks.append(float(time) / 100.0)




# Plotting single timestep results
fig, ax = plt.subplots()
ax.set_title("The Effect of Different Optimisations Upon Simulation Time", size=12)
ax.bar(
        range(len(comparisons)),
        benchmarks,
        tick_label=comparisons,
        align='center', color='k')
ax.set_xlabel("Optimizations", size=12)
ax.set_ylabel("Simulation Run Time (Normalized)\nSmaller is Faster", size=12)
ax.set_ylim([0.0, 0.5])
#ax.set_yscale('log')
fig.savefig('OptimisationComparison.png', dpi=300)
#plt.show()
