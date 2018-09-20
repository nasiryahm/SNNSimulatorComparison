'''
This is a Brian script implementing a benchmark described
in the following review paper:

Simulation of networks of spiking neurons: A review of tools and strategies
(2007). Brette, Rudolph, Carnevale, Hines, Beeman, Bower, Diesmann, Goodman,
Harris, Zirpe, Natschlager, Pecevski, Ermentrout, Djurfeldt, Lansner, Rochel,
Vibert, Alvarez, Muller, Davison, El Boustani and Destexhe.
Journal of Computational Neuroscience 23(3):349-98

Benchmark 2: random network of integrate-and-fire neurons with exponential
synaptic currents.

Clock-driven implementation with exact subthreshold integration
(but spike times are aligned to the grid).
'''
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

from brian2 import *
from scipy.io import mmread

timestep=0.1
defaultclock.dt = timestep*ms

NE = 3200
NI = NE/4

# gl = 10.0*nS
we = 0.4 #*gl
wi = 5.1 #*gl
tau_mem = 20.0*ms
tau_ampa  = 5.0*ms
tau_gaba = 10.0*ms

el = -60.*mV
er = -80.*mV
vt = -50.*mV
vr = -60.*mV
#memc = 200.0*pfarad
bgcurrent = 20.0*mV


eqs = '''
dv/dt  = (-(v-el)-(g_ampa*v+g_gaba*(v-er))+bgcurrent)/tau_mem : volt (unless refractory)
dg_ampa/dt = -g_ampa/tau_ampa : 1
dg_gaba/dt = -g_gaba/tau_gaba : 1
'''

P = NeuronGroup(NE+NI, model=eqs, threshold='v>vt', reset='v = vr', refractory=5.0*ms, method='euler')
P.v = el
P.g_ampa = 0.
P.g_gaba = 0.
Pe = P[:NE]
Pi = P[NE:]

#Ce = Synapses(P, P, on_pre='ge += we')
conn_ee = Synapses(Pe,Pe,model="w:1",on_pre='g_ampa += w', method='euler')
conn_ei = Synapses(Pe,Pi,model="w:1",on_pre='g_ampa += w', method='euler')
conn_ie = Synapses(Pi,Pe,model="w:1",on_pre='g_gaba += w', method='euler')
conn_ii = Synapses(Pi,Pi,model="w:1",on_pre='g_gaba += w', method='euler')


ee_mat = mmread('../ee.wmat')
ei_mat = mmread('../ei.wmat')
ie_mat = mmread('../ie.wmat')
ii_mat = mmread('../ii.wmat')

conn_ee.connect(i=ee_mat.row, j=ee_mat.col)
conn_ee.w[:,:]=we
conn_ee.delay = num_timesteps_min_delay*timestep*ms

conn_ei.connect(i=ei_mat.row, j=ei_mat.col)
conn_ei.w[:,:]=we
conn_ei.delay = num_timesteps_min_delay*timestep*ms

conn_ie.connect(i=ie_mat.row, j=ie_mat.col)
conn_ie.w[:,:]=wi
conn_ie.delay = num_timesteps_min_delay*timestep*ms

conn_ii.connect(i=ii_mat.row, j=ii_mat.col)
conn_ii.w[:,:]=wi
conn_ii.delay = num_timesteps_min_delay*timestep*ms

if (num_timesteps_min_delay != num_timesteps_max_delay):
    delaysetup = str(num_timesteps_min_delay*timestep) + "*ms + " + str((num_timesteps_max_delay - num_timesteps_min_delay)*timestep) + "*ms*rand()"
    conn_ee.delay = delaysetup
    conn_ei.delay = delaysetup
    conn_ie.delay = delaysetup
    conn_ii.delay = delaysetup


#conn_ei.connect(Pe, Pi, scale*array(mmread('../pynn.ei.wmat').todense(),dtype=float))
#conn_ie.connect(Pi, Pe, scale*array(mmread('../pynn.ie.wmat').todense(),dtype=float))
#conn_ii.connect(Pi, Pi, scale*array(mmread('../pynn.ii.wmat').todense(),dtype=float))


print("Simulating for " + str(simtime) + "s")
if (fast):
    starttime = timeit.default_timer()
    run(simtime * second)
    totaltime = timeit.default_timer() - starttime
    f = open("timefile.dat", "w")
    f.write("%f" % totaltime)
    f.close()
    print("Real Time Sim: " + str(totaltime) + "s")
else:
    s_mon = SpikeMonitor(P)
    starttime = timeit.default_timer()
    run(simtime * second)
    totaltime = timeit.default_timer() - starttime
    print("Mean Network Firing Rate: " + str(len(s_mon.i) / (4000.0*simtime)) + "Hz")
    print("Real Time Sim: " + str(totaltime) + "s")
    indices = np.asarray(s_mon.i)
    times = np.asarray(s_mon.t)
    
    # Writing out spikes
    with open("spikes.out", "w") as f:
        for index in range(len(indices)):
            f.write(str(indices[index]) + "\t" + str(times[index]) + "\n")
   #plot(s_mon.t/ms, s_mon.i, ',k')
    #xlabel('Time (ms)')
print("Complete.")
