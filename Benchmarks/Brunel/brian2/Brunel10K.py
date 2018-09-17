from brian2 import *
import random
import getopt, sys, timeit
from scipy.io import mmread

try:
    optlist, args = getopt.getopt(sys.argv[1:], '', ['plastic', 'fast', 'simtime='])
except getopt.GetoptError as err:
    print(str(err))
    sys.exit(2)

simtime = 1.0
fast = False
plastic = False
for o, a in optlist:
    if (o == "--fast"):
        fast = True
        print("Running without Monitoring Spikes (fast mode)\n")
    elif (o == "--simtime"):
        simtime=float(a)
        print("Simulation Time: " + a)
    elif (o == "--plastic"):
        plastic = True
        print("Running with plasticity on\n")

record = False
if (not fast):
    record = True

# ###########################################
# Network parameters
# ###########################################
NE = 8000 # Number of excitatory neurons
NI = 2000 # Number of excitatory neurons
Nrec = 50 # Number of neurons to record
J_ex  = 0.1 # excitatory weight
J_in  = -0.5  # inhibitory weight
p_rate = 20. * Hz # external Poisson rate
delay = 1.5 * ms # synaptic delay
tau_m = 20.0 * ms

taupre = 20.*ms
taupost = 20.*ms
wmax = 0.3 
lbda = 0.01 
alpha = 2.02 

simtime = simtime*second # Simulation time

# ###########################################
# Neuron model
# ###########################################
eqs_neurons='''
dv/dt = -v/tau_m : 1
'''

# ###########################################
# Synapse model
# ###########################################
model='''w:1
         dApre/dt=-Apre/taupre : 1 (event-driven)
         dApost/dt=-Apost/taupost : 1 (event-driven)'''
pre_spike = '''v+=w
         Apre += 1
         w-= lbda * alpha * w * Apost 
'''
post_spike = '''
         Apost += 1
         w += lbda * (wmax - w) * Apre
'''

# ###########################################
# Population
# ###########################################
P = NeuronGroup(NE+NI, model=eqs_neurons, threshold='v>20.', reset='v = 0.', refractory=2.*ms, method='euler')
Pe = P[:NE]
Pi = P[NE:]
P.v = 0.

poisson = PoissonGroup(NE+NI, rates=p_rate)

# ###########################################
# Projections
# ###########################################
print("Creating Projections")
ee_mat = mmread('../ee.wmat')
ei_mat = mmread('../ei.wmat')
ie_mat = mmread('../ie.wmat')
ii_mat = mmread('../ii.wmat')
# EE
if (plastic):
    ee = Synapses(Pe, Pe, model=model, on_pre=pre_spike, on_post=post_spike)
    ee.connect(i=ee_mat.row, j=ee_mat.col)
    # ee.connect(i=np.random.randint(NE, size=(NE*(NE/10))), j=np.repeat(np.arange(NE), (NE/10)))
    ee.w[:,:]='J_ex'
    ee.delay = delay
else:
    ee = Synapses(Pe, Pe, model='w:1', on_pre='v+=w')
    ee.connect(i=ee_mat.row, j=ee_mat.col)
    # ee.connect(i=np.random.randint(NE, size=(NE*(NE/10))), j=np.repeat(np.arange(NE), (NE/10)))
    ee.w[:,:]='J_ex'
    ee.delay = delay
    
# EI
ei = Synapses(Pe, Pi, model='w:1',on_pre='v+=w')
# ei.connect(i=np.random.randint(NE, size=(NI*(NE/10))), j=np.repeat(np.arange(NI), (NE/10)))
ei.connect(i=ei_mat.row, j=ei_mat.col)
ei.w[:,:]='J_ex'
ei.delay = delay

# II
ii = Synapses(Pi, Pi, model='w:1',on_pre='v+=w')
ii.connect(i=ii_mat.row, j=ii_mat.col)
# ii.connect(i=np.random.randint(NI, size=((NE+NI)*(NI/10))), j=np.repeat(np.arange(NE+NI), (NI/10)))
ii.w = J_in
ii.delay = delay

# IE
ie = Synapses(Pi, Pe, model='w:1',on_pre='v+=w')
ie.connect(i=ie_mat.row, j=ie_mat.col)
# ii.connect(i=np.random.randint(NI, size=((NE+NI)*(NI/10))), j=np.repeat(np.arange(NE+NI), (NI/10)))
ie.w = J_in
ie.delay = delay

# External Input
noise = Synapses(poisson, P, model='w:1',on_pre='v+=w')
noise.connect(i=np.random.randint((NE+NI), size=((NE+NI)*((NE+NI)/10))), j=np.repeat(np.arange(NE+NI), ((NE+NI)/10)))
noise.w = J_ex
noise.delay = delay


# ###########################################
# Setting up monitors
# ###########################################
if record:
    sm = SpikeMonitor(P[:Nrec])

# ###########################################
# Simulation
# ###########################################
print 'Start simulation'
starttime = timeit.default_timer()
run(simtime)
totaltime = timeit.default_timer() - starttime
print('Simulation took ' + str(totaltime) + 's')

if (fast):
    with open("./timefile.dat", "w") as f:
        f.write("%f" % totaltime)

if record:
    print 'Mean firing rate:', str(len(sm.i) / (Nrec*simtime)), 'Hz'
    np.savetxt("plasticweights.txt", np.asarray(ee.w))
