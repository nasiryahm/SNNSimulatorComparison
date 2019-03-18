# -*- coding: utf-8 -*-
#
# brunel2000_rand_plastic.py
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.

# This model has been modified by Nasir Ahmad for the purposes of benchmarking

import nest
import nest.raster_plot
import pylab
import numpy
import sys
import getopt, sys, timeit
from scipy.io import mmread
try:
    optlist, args = getopt.getopt(sys.argv[1:], '', ['fast', 'plastic', 'simtime='])
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
        print("Running with plasticity on")


# ###########################################
# Configuration
# ###########################################
record = False
plot_all = False
if (not fast):
    record = True
    plot_all = True
data2file = False # whether to record data to file
N_rec     = 50    # Number of neurons to record from
N_vp      = 1     # number of virtual processes to use

nest.ResetKernel()
dt = 0.1
nest.SetKernelStatus({"print_time": True,
                      "total_num_virtual_procs": N_vp,
                      "resolution": dt})
#                      "overwrite_files": True})

base_seed = 10000  # increase in intervals of at least 2*n_vp+1
n_vp = nest.GetKernelStatus('total_num_virtual_procs')
bs = base_seed # abbreviation to make following code more compact
pyrngs = [numpy.random.RandomState(s) for s in range(bs, bs+n_vp)]
nest.SetKernelStatus({'grng_seed': bs+n_vp,
                      'rng_seeds': range(bs+n_vp+1, bs+1+2*n_vp)})

# ###########################################
# Parameters
# ###########################################
# Network parameters. These are given in Brunel (2000) J.Comp.Neuro.
g       = 5.0    # Ratio of IPSP to EPSP amplitude: J_I/J_E
delay   = 1.5    # synaptic delay in ms
tau_m   = 20.0   # Membrane time constant in mV
V_th    = 20.0   # Spike threshold in mV
simtime = simtime*1000.0 # how long shall we simulate [ms]

N_E = 8000
N_I = 2000
N_neurons = N_E+N_I

C_E    = int(N_E/10) # number of excitatory synapses per neuron
C_I    = int(N_I/10) # number of inhibitory synapses per neuron

J_E  = 0.1
J_I  = -g*J_E

# Synaptic parameters
STDP_alpha   = 2.02     # relative strength of STDP depression w.r.t potentiation 
STDP_Wmax = 3*J_E       #maximum weight of plastic synapse

# ###########################################
# Neuron model
# ###########################################
nest.SetDefaults("iaf_psc_delta", 
                 {"C_m": 1.0,
                  "tau_m": tau_m,
                  "t_ref": 2.0,
                  "E_L": 0.0,
                  "V_th": V_th,
                  "V_m": 0.0,
                  "tau_minus":20.0,
                  "V_reset": 0.0,
                  })
print nest.GetDefaults("iaf_psc_delta")

# ###########################################
# Populations
# ###########################################
nodes   = nest.Create("iaf_psc_delta",N_neurons)
nodes_E = nodes[:N_E]
nodes_I = nodes[N_E:]

'''
# randomize membrane potential
node_E_info = nest.GetStatus(nodes_E, ['local','global_id','vp'])
node_I_info = nest.GetStatus(nodes_I, ['local','global_id','vp'])
local_E_nodes = [(gid,vp) for islocal,gid,vp in node_E_info if islocal]
local_I_nodes = [(gid,vp) for islocal,gid,vp in node_I_info if islocal]
for gid,vp in local_E_nodes + local_I_nodes: 
  nest.SetStatus([gid], {'V_m': pyrngs[vp].uniform(-V_th,0.95*V_th)})
'''

# Poisson input
noise = nest.Create("poisson_generator", 1,{"rate": 20000.}) 
# noise_parrot = nest.Create("parrot_neuron", 10000) 

print nest.GetDefaults("poisson_generator")
# ###########################################
# Synapse models
# ###########################################
nest.CopyModel("stdp_synapse_hom",
               "excitatory-plastic",
               {"alpha":STDP_alpha,
                "Wmax":STDP_Wmax,
                "weight":J_E,
                "delay":delay})

nest.CopyModel("static_synapse_hom_w",
               "inhibitory",
               {"weight":J_I, 
                "delay":delay})

nest.CopyModel("static_synapse_hom_w",
               "excitatory-input",
               {"weight":J_E, "delay":delay})

# ###########################################
# Projections
# ###########################################
A=mmread('../ee.wmat')
rows,cols = A.nonzero()
if (plastic):
    nest.Connect(rows+1, cols+1, {"rule": "one_to_one"},  model="excitatory-plastic")
else:
    nest.Connect(rows+1, cols+1, {"rule": "one_to_one"},  model="excitatory-input")

A=mmread('../ei.wmat')
rows,cols = A.nonzero()
nest.Connect(rows+1, cols+N_E+1, {"rule": "one_to_one"},  model="excitatory-input")

A=mmread('../ie.wmat')
rows,cols = A.nonzero()
nest.Connect(rows+1+N_E, cols+1, {"rule": "one_to_one"}, syn_spec="inhibitory")

A=mmread('../ii.wmat')
rows,cols = A.nonzero()
nest.Connect(rows+1+N_E, cols+N_E+1, {"rule": "one_to_one"},  model="inhibitory")

# Input stimulation
#nest.Connect(noise, noise_parrot, {"rule": 'one_to_one'})
nest.Connect(noise, nodes, 
        {"rule": 'fixed_indegree', "indegree": 1}, #(C_E + C_I)}, 
             {"model": "excitatory-input", "delay": delay})


if record:
    spikes=nest.Create("spike_detector",1, 
                       [{"label": "brunel-py-ex", "to_file": data2file}])
    parrotspikes=nest.Create("spike_detector",1, 
                       [{"label": "brunel-py-ex", "to_file": data2file}])

    # connect using all_to_all: all recorded excitatory neurons to one detector
    nest.Connect(nodes_E[:N_rec], spikes, 'all_to_all')
    #nest.Connect(noise, parrotspikes, 'all_to_all')

# ###########################################
# Simulation
# ###########################################
starttime = timeit.default_timer()
nest.Simulate(simtime)
totaltime = timeit.default_timer() - starttime
print 'Simulating', simtime/1000.0, 's took', totaltime, 'seconds.'

if (fast):
    with open("timefile.dat", "w") as f:
        f.write("%f" % totaltime)

# ###########################################
# Data analysis
# ###########################################
if record:
    events = nest.GetStatus(spikes,"n_events")
    N_rec_local_E = sum(nest.GetStatus(nodes_E[:N_rec], 'local'))
    rate_ex= 1000.0 * events[0]/(simtime*N_rec_local_E)
    print("Excitatory rate   : %.2f Hz" % rate_ex)
    events = nest.GetStatus(parrotspikes,"n_events")
    rate_ex= 1000.0 * events[0]/(simtime)
    print("Excitatory rate   : %.2f Hz" % rate_ex)
    w = nest.GetStatus(nest.GetConnections(
      synapse_model='excitatory-plastic'),
      'weight')
    numpy.savetxt("PlasticSynapseWeights.txt", w)
    #if plot_all:
    #    nest.raster_plot.from_device(spikes, hist=True)
    #    pylab.show()

#if plot_all:
#  pylab.figure()
#  pylab.hist(w, bins=100)
#  pylab.xlabel('Synaptic weight [pA]')
#  pylab.show()
