#include <cmath>
#include <vector>

#include "modelSpec.h"

// GeNN robotics includes
//#include "genn_models/exp_curr.h"
//#include "exp_curr.h"
#include "lif.h"
#include "stdp_multiplicative.h"
//#include "connectors.h"

#include "parameters.h"

void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(0.1);
    model.setName("brunel_benchmark");

    GENN_PREFERENCES::autoInitSparseVars = true;
    GENN_PREFERENCES::defaultVarMode = VarMode::LOC_HOST_DEVICE_INIT_HOST;

    //---------------------------------------------------------------------------
    // Build model
    //---------------------------------------------------------------------------
    InitVarSnippet::Uniform::ParamValues vDist(
        Parameters::resetVoltage,       // 0 - min
        Parameters::thresholdVoltage);  // 1 - max

    // LIF model parameters
    BoBRobotics::GeNNModels::LIF::ParamValues lifParams(
        200.0e-9,    // 0 - C
        20.0,   // 1 - TauM
        Parameters::restVoltage,  // 2 - Vrest
        Parameters::resetVoltage,  // 3 - Vreset
        Parameters::thresholdVoltage,  // 4 - Vthresh
        0.0,    // 5 - Ioffset
        0.0);    // 6 - TauRefrac

    // LIF initial conditions
    BoBRobotics::GeNNModels::LIF::VarValues lifInit(
        Parameters::restVoltage, //initVar<InitVarSnippet::Uniform>(vDist),     // 0 - V
        0.0);   // 1 - RefracTime

    NeuronModels::PoissonNew::VarValues poisInit(
        0.0f // 0 - Membrane Voltage
        );
        //42, // 1 - Seed
        //100.0 // 2 - Last Spike Time of Neuron
        //);
    NeuronModels::PoissonNew::ParamValues poisParams(
        20.0f // 0 - Firing Rate
        );
        //0.0f, // 1 - Refractory Period
        //20.0, // 2 - Threshold Voltage for Spike
        //0.0 // 3 - Rest Voltage
        //);
    auto *poisson = model.addNeuronPopulation<NeuronModels::PoissonNew>("P", Parameters::numPoisson, poisParams, poisInit);

    // Create IF_curr neuron
    auto *e = model.addNeuronPopulation<BoBRobotics::GeNNModels::LIF>("E", Parameters::numExcitatory, lifParams, lifInit);
    auto *i = model.addNeuronPopulation<BoBRobotics::GeNNModels::LIF>("I", Parameters::numInhibitory, lifParams, lifInit);

    STDPWeightDependent::VarValues stdp_ini(
          Parameters::excitatoryWeight, // 0 - g: the synaptic conductance value
          0.0, // pretrace
          0.0, // t_preupdate
          0.0, // posttrace
          0.0 // t_postupdate
    );
    STDPWeightDependent::ParamValues stdp_params(
      20.0,  // 0 - Potentiation time constant (ms)
      20.0, // 1 - Depression time constant (ms)
      1.0,    // 2 - Rate of potentiation
      1.0,   // 3 - Rate of depression
      0.0,     // 4 - Minimum weight
      3.0f*Parameters::excitatoryWeight,     // 5 - Maximum weight
      0.01,   // 6 - Learning Rate
      2.02    // 7 - Relative Weighting (LTD to LTP)
    );

    
    WeightUpdateModels::StaticPulse::VarValues excs_ini(
          Parameters::excitatoryWeight // 0 - g: the synaptic conductance value
    );
    WeightUpdateModels::StaticPulse::VarValues inhibs_ini(
          Parameters::inhibitoryWeight // 0 - g: the synaptic conductance value
    );

    /*
    PostsynapticModels::DeltaCurr::ParamValues excitatorySyns(
          Parameters::excitatoryWeight // 0 - g: the synaptic conductance value
        );
    PostsynapticModels::DeltaCurr::ParamValues inhibitorySyns(
          Parameters::inhibitoryWeight // 0 - g: the synaptic conductance value
        );
        */

    int DELAY = Parameters::synapticDelay; // In timesteps
    auto *pe = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>(
        "PE", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "P", "E",
        {}, excs_ini,
        {},
        {});
    pe->setMaxConnections(Parameters::probabilityConnection*Parameters::numExcitatory);
    auto *pi = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>(
        "PI", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "P", "I",
        {}, excs_ini,
        {},
        {});
    pi->setMaxConnections(Parameters::probabilityConnection*Parameters::numInhibitory);

    auto *ee = model.addSynapsePopulation<STDPWeightDependent, PostsynapticModels::DeltaCurr>(
    //auto *ee = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>( // Uncomment for no STDP
        "EE", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "E", "E",
        stdp_params, stdp_ini,
        //{}, excs_ini, //Uncomment for no STDP
        {}, {});
    ee->setMaxConnections(Parameters::EEMaxRow);

    auto *ei = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>(
        "EI", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "E", "I",
        {}, excs_ini,
        {}, {});
    ei->setMaxConnections(Parameters::EIMaxRow);

    auto *ii = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>(
        "II", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "I", "I",
        {}, inhibs_ini,
        {}, {});
    ii->setMaxConnections(Parameters::IIMaxRow);

    auto *ie = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::DeltaCurr>(
        "IE", SynapseMatrixType::RAGGED_INDIVIDUALG, DELAY,
        "I", "E",
        {}, inhibs_ini,
        {}, {});
    ie->setMaxConnections(Parameters::IEMaxRow);


    // Configure spike variables so that they can be downloaded to host
    poisson->setSpikeVarMode(VarMode::LOC_HOST_DEVICE_INIT_DEVICE);
    e->setSpikeVarMode(VarMode::LOC_HOST_DEVICE_INIT_DEVICE);
    i->setSpikeVarMode(VarMode::LOC_HOST_DEVICE_INIT_DEVICE);

    model.finalize();
}
