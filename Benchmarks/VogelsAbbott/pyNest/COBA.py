# Vogels Abbott Benchmark

import getopt, sys, timeit
try:
    optlist, args = getopt.getopt(sys.argv[1:], '', ['fast', 'simtime=', 'num_timesteps_delay='])
except getopt.GetoptError as err:
    print(str(err))
    sys.exit(2)

simtime = 1.0
fast = False
num_timesteps_delay = 1
for o, a in optlist:
    if (o == "--fast"):
        fast = True
        print("Running without Monitoring Spikes (fast mode)\n")
    elif (o == "--simtime"):
        simtime=float(a)
        print("Simulation Time: " + a)
    elif (o == "--num_timesteps_delay"):
        num_timesteps_delay=int(a)
        print("Delay (in number of timesteps): " + a)

import nest
import numpy
from scipy.io import mmread

NE = 3200
NI = NE / 4
N_total = NE + NI
N_rec = NE

dt = 0.1
delay = dt*num_timesteps_delay
J_E = 4.0
J_I = -51.0

nest.SetKernelStatus({"local_num_threads": 1, "resolution": dt, "overwrite_files": True})
simtime=simtime*1000.0
#nest.SetKernelStatus({
#    "printtime": False,
#    "localnumthreads": 1,
#    "overwritefiles": True})

synapse_model = "static_synapse"
neuron_model = "iaf_cond_exp"

neuron_params = { 
          "V_m": -60.0,
          "E_L": -60.0,
          "V_th": -50.0,
          "V_reset": -60.0,
          "t_ref": 5.0,
          "E_ex": 0.0,
          "E_in": -80.0,
          "C_m": 200.0,
          "g_L": 10.0,
          "I_e": 200.0,
          "tau_syn_ex": 5.0,
          "tau_syn_in": 10.0}

nodes = nest.Create(neuron_model, N_total, params=neuron_params)
nodes_E = nodes[:NE]
nodes_I = nodes[NE:]

nest.SetStatus(nodes, "V_m", -60.0)



# Setting up Synapses
p = 0.02
exc_syn_dict ={
        "model": "static_synapse",
        "weight": J_E,
        "delay": delay }
inh_syn_dict ={
        "model": "static_synapse",
        "weight":J_I,
        "delay": delay }

#prob_conn_dict = {"rule": "pairwise_bernoulli", "p": p}

#nest.Connect(nodes_E, nodes, syn_spec=exc_syn_dict, conn_spec=prob_conn_dict)
#nest.Connect(nodes_I, nodes, syn_spec=inh_syn_dict, conn_spec=prob_conn_dict)

conn_dict = {"rule": "one_to_one"}
A = mmread('../ee.wmat')
#rows, cols = A.nonzero()
nest.Connect(pre=A.row+1, post=A.col+1, syn_spec=exc_syn_dict, conn_spec=conn_dict)
A = mmread('../ei.wmat')
#rows, cols = A.nonzero()
nest.Connect(A.row+1, A.col+NE+1, syn_spec=exc_syn_dict, conn_spec=conn_dict)
A = mmread('../ie.wmat')
#rows, cols = A.nonzero()
nest.Connect(A.row+NE+1, A.col+1, syn_spec=inh_syn_dict, conn_spec=conn_dict)
A = mmread('../ii.wmat')
#rows, cols = A.nonzero()
nest.Connect(A.row+NE+1, A.col+NE+1, syn_spec=inh_syn_dict, conn_spec=conn_dict)

if (not fast):
    spikes = nest.Create("spike_detector", 1, 
                [{"label": "va-py-ex", "to_file": True}])
    spikes_E = spikes[:1]
    nest.Connect(nodes_E[:N_rec], spikes_E)

starttime = timeit.default_timer()
nest.Simulate(simtime)
totaltime = timeit.default_timer() - starttime
print("Real Time Sim: " + str(totaltime) + "s")

if (fast):
    f = open("timefile.dat", "w")
    f.write("%f" % totaltime)
    f.close()

if (not fast):
    rate_iaf = nest.GetStatus(spikes)[0][
                        "n_events"] / ((simtime / 1000.0) * N_rec)
    print("Average Rate of recorded electrodes: " + str(rate_iaf) + "Hz")
