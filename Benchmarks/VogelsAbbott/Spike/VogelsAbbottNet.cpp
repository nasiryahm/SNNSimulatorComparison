// Vogels Abbot Benchmark Network
// Author: Nasir Ahmad (Created: 16/09/2016)


/*
  This network has been created to benchmark Spike. It shall follow the network
  used to analyse Auryn.

  Publications:
  Vogels, Tim P., and L. F. Abbott. 2005. “Signal Propagation and Logic Gating in Networks of Integrate-and-Fire Neurons.” The Journal of Neuroscience: The Official Journal of the Society for Neuroscience 25 (46): 10786–95.
  Zenke, Friedemann, and Wulfram Gerstner. 2014. “Limits to High-Speed Simulations of Spiking Neural Networks Using General-Purpose Computers.” Frontiers in Neuroinformatics 8 (August). Frontiers. doi:10.3389/fninf.2014.00076.

*/


#include "Spike/Spike.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <getopt.h>
#include <time.h>
#include <iomanip>
#include <vector>

void connect_from_mat(
    int layer1,
    int layer2,
    conductance_spiking_synapse_parameters_struct* SYN_PARAMS, 
    std::string filename,
    SpikingModel* Model,
    float timestep){

  ifstream weightfile;
  string line;
  stringstream ss;
  std::vector<int> prevec, postvec;
  std::vector<float> weightvec;
  std::vector<float> delayvec;
  int pre, post;
  float weight;
  int linecount = 0;
  weightfile.open(filename.c_str());

  if (weightfile.is_open()){
    printf("Loading weights from mat file: %s\n", filename.c_str());
    while (getline(weightfile, line)){
      if (line.c_str()[0] == '%'){
        continue;
      } else {
        linecount++;
        if (linecount == 1) continue; // Ignore Header
        ss.clear();
        ss << line;
        ss >> pre >> post >> weight;
        prevec.push_back(pre - 1);
        postvec.push_back(post - 1);
        weightvec.push_back(weight);
        delayvec.push_back(SYN_PARAMS->delay_range[0]);
      }
    }
    SYN_PARAMS->pairwise_connect_presynaptic = prevec;
    SYN_PARAMS->pairwise_connect_postsynaptic = postvec;
    SYN_PARAMS->pairwise_connect_weight = weightvec;
    SYN_PARAMS->pairwise_connect_delay = delayvec;
    SYN_PARAMS->connectivity_type = CONNECTIVITY_TYPE_PAIRWISE;
    Model->AddSynapseGroup(layer1, layer2, SYN_PARAMS);
  } else {
    printf("Could not load connectivity matrix: %s\n", filename.c_str());
    exit(-1);
  }
}


int main (int argc, char *argv[]){
  // Getting options:
  float simtime = 20.0;
  bool fast = false;
  bool no_TG = false;
  int num_timesteps_delay = 8;
  int networkscale = 1;

  const char* const short_opts = "";
  const option long_opts[] = {
    {"simtime", 1, nullptr, 0},
    {"fast", 0, nullptr, 1},
    {"num_timesteps_delay", 1, nullptr, 2},
    {"NOTG", 0, nullptr, 3},
    {"networkscale", 1, nullptr, 4}
  };
  // Check the set of options
  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    // If none
    if (-1 == opt) break;

    switch (opt){
      case 0:
        printf("Running with a simulation time of: %ss\n", optarg);
        simtime = std::stof(optarg);
        break;
      case 1:
        printf("Running in fast mode (no spike collection)\n");
        fast = true;
        break;
      case 2:
        printf("Running with delay: %s timesteps\n", optarg);
        num_timesteps_delay = std::stoi(optarg);
        break;
      case 3:
        printf("TURNING OFF TIMESTEP GROUPING\n");
        no_TG = true;
        break;
      case 4:
        printf("Running with Network Scaled by: %s\n", optarg);
        networkscale = std::stoi(optarg);
        break;
    }
  };
  
  // TIMESTEP MUST BE SET BEFORE DATA IS IMPORTED. USED FOR ROUNDING.
  // The details below shall be used in a SpikingModel
  SpikingModel * BenchModel = new SpikingModel();
  float timestep = 0.0001f; // 50us for now
  BenchModel->SetTimestep(timestep);

  // Create neuron, synapse and stdp types for this model
  LIFSpikingNeurons * lif_spiking_neurons = new LIFSpikingNeurons();
  ConductanceSpikingSynapses * conductance_spiking_synapses = new ConductanceSpikingSynapses();
  // Add my populations to the SpikingModel
  BenchModel->spiking_neurons = lif_spiking_neurons;
  BenchModel->spiking_synapses = conductance_spiking_synapses;
  
  // Add a monitor for Neuron Spiking
  SpikingActivityMonitor* spike_monitor = new SpikingActivityMonitor(lif_spiking_neurons);
  if (!fast)
    BenchModel->AddActivityMonitor(spike_monitor);

  // Set up Neuron Parameters
  lif_spiking_neuron_parameters_struct * EXC_NEURON_PARAMS = new lif_spiking_neuron_parameters_struct();
  lif_spiking_neuron_parameters_struct * INH_NEURON_PARAMS = new lif_spiking_neuron_parameters_struct();

  EXC_NEURON_PARAMS->somatic_capacitance_Cm = 20.0f*pow(10.0, -3);//200.0f*pow(10.0, -12);  // pF
  INH_NEURON_PARAMS->somatic_capacitance_Cm = 20.0f*pow(10.0, -3);//200.0f*pow(10.0, -12);  // pF

  EXC_NEURON_PARAMS->somatic_leakage_conductance_g0 = 1.0f;//10.0f*pow(10.0, -9);  // nS
  INH_NEURON_PARAMS->somatic_leakage_conductance_g0 = 1.0f;//10.0f*pow(10.0, -9);  // nS

  EXC_NEURON_PARAMS->resting_potential_v0 = -60.0f*pow(10.0, -3);
  INH_NEURON_PARAMS->resting_potential_v0 = -60.0f*pow(10.0, -3);
  
  EXC_NEURON_PARAMS->after_spike_reset_potential_vreset = -60.0f*pow(10.0, -3);
  INH_NEURON_PARAMS->after_spike_reset_potential_vreset = -60.0f*pow(10.0, -3);

  EXC_NEURON_PARAMS->absolute_refractory_period = 5.0f*pow(10, -3);  // ms
  INH_NEURON_PARAMS->absolute_refractory_period = 5.0f*pow(10, -3);  // ms

  EXC_NEURON_PARAMS->threshold_for_action_potential_spike = -50.0f*pow(10.0, -3); // -53mV threshold
  INH_NEURON_PARAMS->threshold_for_action_potential_spike = -50.0f*pow(10.0, -3); // -53mV threshold

  EXC_NEURON_PARAMS->background_current = 2.0f*pow(10.0, -2); //
  INH_NEURON_PARAMS->background_current = 2.0f*pow(10.0, -2); //

  /*
    Setting up NEURON POPULATION
  */
  vector<int> EXCITATORY_NEURONS;
  vector<int> INHIBITORY_NEURONS;
  // Creating a single exc and inh population for now
  EXC_NEURON_PARAMS->group_shape[0] = 1;
  EXC_NEURON_PARAMS->group_shape[1] = 3200*networkscale;
  INH_NEURON_PARAMS->group_shape[0] = 1;
  INH_NEURON_PARAMS->group_shape[1] = 800*networkscale;
  EXCITATORY_NEURONS.push_back(BenchModel->AddNeuronGroup(EXC_NEURON_PARAMS));
  INHIBITORY_NEURONS.push_back(BenchModel->AddNeuronGroup(INH_NEURON_PARAMS));

  /*
    Setting up SYNAPSES
  */
  conductance_spiking_synapse_parameters_struct * EXC_OUT_SYN_PARAMS = new conductance_spiking_synapse_parameters_struct();
  conductance_spiking_synapse_parameters_struct * INH_OUT_SYN_PARAMS = new conductance_spiking_synapse_parameters_struct();
  // Setting delays
  EXC_OUT_SYN_PARAMS->delay_range[0] = num_timesteps_delay*timestep;
  EXC_OUT_SYN_PARAMS->delay_range[1] = num_timesteps_delay*timestep;
  INH_OUT_SYN_PARAMS->delay_range[0] = num_timesteps_delay*timestep;
  INH_OUT_SYN_PARAMS->delay_range[1] = num_timesteps_delay*timestep;
  // Setting Reversal Potentials for specific synapses (according to evans paper)
  EXC_OUT_SYN_PARAMS->reversal_potential_Vhat = 0.0f*pow(10.0, -3);
  INH_OUT_SYN_PARAMS->reversal_potential_Vhat = -80.0f*pow(10.0, -3);
  // Set timescales
  EXC_OUT_SYN_PARAMS->decay_term_tau_g = 5.0f*pow(10.0, -3);  // 5ms
  INH_OUT_SYN_PARAMS->decay_term_tau_g = 10.0f*pow(10.0, -3);  // 10ms

  // Biological Scaling factors
  EXC_OUT_SYN_PARAMS->weight_scaling_constant = EXC_NEURON_PARAMS->somatic_leakage_conductance_g0; //10.0f*pow(10.0,-9);
  INH_OUT_SYN_PARAMS->weight_scaling_constant = INH_NEURON_PARAMS->somatic_leakage_conductance_g0; // 10.0f*pow(10.0,-9);
  /*
  EXC_OUT_SYN_PARAMS->weight_range[0] = 0.4;
  EXC_OUT_SYN_PARAMS->weight_range[1] = 0.4;
  INH_OUT_SYN_PARAMS->weight_range[0] = 5.1;
  INH_OUT_SYN_PARAMS->weight_range[1] = 5.1;
  */

  /*
  // Creating Synapse Populations
  EXC_OUT_SYN_PARAMS->connectivity_type = CONNECTIVITY_TYPE_RANDOM;
  INH_OUT_SYN_PARAMS->connectivity_type = CONNECTIVITY_TYPE_RANDOM;
  INPUT_SYN_PARAMS->connectivity_type = CONNECTIVITY_TYPE_RANDOM;
  EXC_OUT_SYN_PARAMS->plasticity_vec.push_back(nullptr);
  INH_OUT_SYN_PARAMS->plasticity_vec.push_back(nullptr);
  INPUT_SYN_PARAMS->plasticity_vec.push_back(nullptr);
  EXC_OUT_SYN_PARAMS->random_connectivity_probability = 0.02; // 2%
  INH_OUT_SYN_PARAMS->random_connectivity_probability = 0.02; // 2%
  INPUT_SYN_PARAMS->random_connectivity_probability = 0.01; // 1%

  // Connect all of the populations
  BenchModel->AddSynapseGroup(EXCITATORY_NEURONS[0], EXCITATORY_NEURONS[0], EXC_OUT_SYN_PARAMS);
  BenchModel->AddSynapseGroup(EXCITATORY_NEURONS[0], INHIBITORY_NEURONS[0], EXC_OUT_SYN_PARAMS);
  BenchModel->AddSynapseGroup(INHIBITORY_NEURONS[0], EXCITATORY_NEURONS[0], INH_OUT_SYN_PARAMS);
  BenchModel->AddSynapseGroup(INHIBITORY_NEURONS[0], INHIBITORY_NEURONS[0], INH_OUT_SYN_PARAMS);
  //BenchModel->AddSynapseGroup(input_layer_ID, EXCITATORY_NEURONS[0], INPUT_SYN_PARAMS);
  */

  // Adding connections based upon matrices given
  std::string connFile = "../../ee.wmat";
  if (networkscale != 1){
    connFile = "../../auryn/" + std::to_string(networkscale) + ".0.0.wmat";
  }
  connect_from_mat(
      EXCITATORY_NEURONS[0], EXCITATORY_NEURONS[0],
      EXC_OUT_SYN_PARAMS, 
      connFile.c_str(),
      BenchModel,
      timestep);

  connFile = "../../ei.wmat";
  if (networkscale != 1){
    connFile = "../../auryn/" + std::to_string(networkscale) + ".1.0.wmat";
  }
  connect_from_mat(
    EXCITATORY_NEURONS[0], INHIBITORY_NEURONS[0],
    EXC_OUT_SYN_PARAMS, 
    connFile.c_str(),
    BenchModel,
    timestep);
  
  connFile = "../../ie.wmat";
  if (networkscale != 1){
    connFile = "../../auryn/" + std::to_string(networkscale) + ".2.0.wmat";
  }
  connect_from_mat(
    INHIBITORY_NEURONS[0], EXCITATORY_NEURONS[0],
    INH_OUT_SYN_PARAMS, 
    connFile.c_str(),
    BenchModel,
    timestep);
  
  connFile = "../../ii.wmat";
  if (networkscale != 1){
    connFile = "../../auryn/" + std::to_string(networkscale) + ".3.0.wmat";
  }
  connect_from_mat(
    INHIBITORY_NEURONS[0], INHIBITORY_NEURONS[0],
    INH_OUT_SYN_PARAMS, 
    connFile.c_str(),
    BenchModel,
    timestep);




  /*
    COMPLETE NETWORK SETUP
  */
  BenchModel->finalise_model();
  if (no_TG)
    BenchModel->timestep_grouping = 1;

  clock_t starttime = clock();
  BenchModel->run(simtime);
  clock_t totaltime = clock() - starttime;
  if ( fast ){
    std::ofstream timefile;
    timefile.open("timefile.dat");
    timefile << std::setprecision(10) << ((float)totaltime / CLOCKS_PER_SEC);
    timefile.close();
  } else {
    //spike_monitor->save_spikes_as_txt("./");
    spike_monitor->save_spikes_as_binary("./", "VA");
  }
  return(0);
}
