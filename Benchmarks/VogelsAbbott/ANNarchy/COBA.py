import getopt, sys
import time
try:
    optlist, args = getopt.getopt(sys.argv[1:], '', ['fast', 'simtime='])
except getopt.GetoptError as err:
    print(str(err))
    usage()
    sys.exit(2)

simtime = 1.0
fast = False
for o, a in optlist:
    if (o == "--fast"):
        fast = True
        print("Running without Monitoring Spikes (fast mode)\n")
    elif (o == "--simtime"):
        simtime=float(a)
        print("Simulation Time: " + a)


from ANNarchy import *
setup(dt=0.1)
# ###########################################
# Neuron model
# ###########################################
COBA = Neuron(
    parameters="""
        El = -60.0          : population
        Vr = -60.0          : population
        Erev_exc = 0.0      : population
        Erev_inh = -80.0    : population
        Vt = -50.0          : population
        tau = 20.0          : population
        tau_exc = 5.0       : population
        tau_inh = 10.0      : population
        I = 20.0            : population
    """,
    equations="""
        tau * dv/dt = (El - v) + g_exc * (Erev_exc - v) + g_inh * (Erev_inh - v ) + I

        tau_exc * dg_exc/dt = - g_exc
        tau_inh * dg_inh/dt = - g_inh
    """,
    spike = "v > Vt",
    reset = "v = Vr",
    refractory = 5.0
)

# ###########################################
# Create population
# ###########################################
P = Population(geometry=4000, neuron=COBA)
Pe = P[:3200]
Pi = P[3200:]
P.v = Normal(-55.0, 5.0)
P.g_exc = Normal(4.0, 1.5)
P.g_inh = Normal(20.0, 12.0)

# ###########################################
# Connect the network
# ###########################################
Ce = Projection(pre=Pe, post=P, target='exc')
Ce.connect_fixed_probability(weights=0.6, probability=0.02)
Ci = Projection(pre=Pi, post=P, target='inh')
Ci.connect_fixed_probability(weights=6.7, probability=0.02)

compile()

# ###########################################
# Simulate
# ###########################################
if (fast):
    starttime = time.clock()
    simulate(simtime*1000.0, measure_time=True)
    totaltime = time.clock() - starttime
    f = open("timefile.dat", "w")
    f.write("%f" % totaltime)
    f.close()
else:
    m = Monitor(P, ['spike'])
    simulate(simtime*1000.0, measure_time=True)
    data = m.get('spike')

    # ###########################################
    # Make plots
    # ###########################################
    t, n = m.raster_plot(data)
    print('Mean firing rate in the population: ' + str(len(t) / 4000.) + 'Hz')

#import matplotlib.pyplot as plt
#plt.plot(t, n, '.', markersize=0.5)
#plt.xlabel('Time (ms)')
#plt.ylabel('# neuron')
#plt.show()
