import getopt, sys, timeit
try:
    optlist, args = getopt.getopt(sys.argv[1:], '', ['fast', 'simtime=', 'num_timesteps_min_delay=', 'num_timesteps_max_delay='])
except getopt.GetoptError as err:
    print(str(err))
    sys.exit(2)

simtime = 1.0
fast = False
num_timesteps_min_delay = 1
num_timesteps_max_delay = 1
for o, a in optlist:
    if (o == "--fast"):
        fast = True
        print("Running without Monitoring Spikes (fast mode)\n")
    elif (o == "--simtime"):
        simtime=float(a)
        print("Simulation Time: " + a)
    elif (o == "--num_timesteps_min_delay"):
        num_timesteps_min_delay=int(a)
        if (num_timesteps_max_delay < num_timesteps_min_delay):
            num_timesteps_max_delay = num_timesteps_min_delay
        print("Minimum delay (in number of timesteps): " + a)
    elif (o == "--num_timesteps_max_delay"):
        num_timesteps_max_delay=int(a)
        if (num_timesteps_max_delay < num_timesteps_min_delay):
            print("ERROR: Max delay should not be smaller than min!")
            exit(1)
        print("Maximum delay (in number of timesteps): " + a)


from ANNarchy import *
timestep = 0.1 # in ms
setup(dt=timestep)
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
P.v = -55.0#Normal(-55.0, 5.0)
P.g_exc = 0.0#Normal(4.0, 1.5)
P.g_inh = 0.0#Normal(20.0, 12.0)

# ###########################################
# Connect the network
# ###########################################
gleak = 1.0; #1000.0 * (500.0*pow(10.0, -12.0) / 0.02)
if (num_timesteps_min_delay != num_timesteps_max_delay):
    Ce = Projection(pre=Pe, post=P, target='exc')
    Ce.connect_fixed_probability(weights=0.4*gleak, probability=0.02, delays=Uniform(num_timesteps_min_delay*timestep, num_timesteps_max_delay*timestep))
    Ci = Projection(pre=Pi, post=P, target='inh')
    Ci.connect_fixed_probability(weights=5.1*gleak, probability=0.02, delays=Uniform(num_timesteps_min_delay*timestep, num_timesteps_max_delay*timestep))
else:
    Ce = Projection(pre=Pe, post=P, target='exc')
    Ce.connect_fixed_probability(weights=0.4*gleak, probability=0.02, delays=num_timesteps_min_delay*timestep)
    Ci = Projection(pre=Pi, post=P, target='inh')
    Ci.connect_fixed_probability(weights=5.1*gleak, probability=0.02, delays=num_timesteps_min_delay*timestep)

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
    print('Mean firing rate in the population: ' + str(len(t) / (simtime*4000.)) + 'Hz')

#import matplotlib.pyplot as plt
#plt.plot(t, n, '.', markersize=0.5)
#plt.xlabel('Time (ms)')
#plt.ylabel('# neuron')
#plt.show()
