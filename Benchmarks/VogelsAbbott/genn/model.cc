#include <cmath>
#include <vector>

#include "modelSpec.h"

// GeNN robotics includes
//#include "genn_models/exp_curr.h"
//#include "exp_curr.h"
#include "lif.h"
#include "connectors.h"

#include "parameters.h"

using namespace BoBRobotics;

void modelDefinition(NNmodel &model)
{
    initGeNN();
    model.setDT(0.1);
    model.setName("va_benchmark");

    GENN_PREFERENCES::autoInitSparseVars = true;
    GENN_PREFERENCES::defaultVarMode = VarMode::LOC_DEVICE_INIT_DEVICE;

    //---------------------------------------------------------------------------
    // Build model
    //---------------------------------------------------------------------------
    InitVarSnippet::Uniform::ParamValues vDist(
        Parameters::resetVoltage,       // 0 - min
        Parameters::thresholdVoltage);  // 1 - max

    // LIF model parameters
    GeNNModels::LIF::ParamValues lifParams(
        20.0,    // 0 - C
        20.0,   // 1 - TauM
        Parameters::restVoltage,  // 2 - Vrest
        Parameters::resetVoltage,  // 3 - Vreset
        Parameters::thresholdVoltage,  // 4 - Vthresh
        20.0,    // 5 - Ioffset
        5.0);    // 6 - TauRefrac

    // LIF initial conditions
    GeNNModels::LIF::VarValues lifInit(
        Parameters::restVoltage, //initVar<InitVarSnippet::Uniform>(vDist),     // 0 - V
        0.0);   // 1 - RefracTime

    // Create IF_curr neuron
    auto *e = model.addNeuronPopulation<GeNNModels::LIF>("E", Parameters::numExcitatory, lifParams, lifInit);
    auto *i = model.addNeuronPopulation<GeNNModels::LIF>("I", Parameters::numInhibitory, lifParams, lifInit);


    WeightUpdateModels::StaticPulse::VarValues excs_ini(
            Parameters::excitatoryWeight // 0 - g: the synaptic conductance value
    );
    WeightUpdateModels::StaticPulse::VarValues inhibs_ini(
            Parameters::inhibitoryWeight // 0 - g: the synaptic conductance value
    );

    PostsynapticModels::ExpCond::ParamValues excitatorySyns(
            5.0,      // 0 - tau_S: decay time constant for S [ms]
            -0.0     // 1 - Erev: Reversal potential
    );
    PostsynapticModels::ExpCond::ParamValues InhibitorySyns(
            10.0,      // 0 - tau_S: decay time constant for S [ms]
            -80.0     // 1 - Erev: Reversal potential
    );

    int DELAY = 8; // In timesteps
    auto *ee = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "EE", SynapseMatrixType::DENSE_GLOBALG_INDIVIDUAL_PSM, DELAY,
        "E", "E",
        {}, excs_ini,
        excitatorySyns, {});
    auto *ei = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "EI", SynapseMatrixType::DENSE_GLOBALG_INDIVIDUAL_PSM, DELAY,
        "E", "I",
        {}, excs_ini,
        excitatorySyns, {});
    auto *ii = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "II", SynapseMatrixType::DENSE_GLOBALG_INDIVIDUAL_PSM, DELAY,
        "I", "I",
        {}, inhibs_ini,
        InhibitorySyns, {});
    auto *ie = model.addSynapsePopulation<WeightUpdateModels::StaticPulse, PostsynapticModels::ExpCond>(
        "IE", SynapseMatrixType::DENSE_GLOBALG_INDIVIDUAL_PSM, DELAY,
        "I", "E",
        {}, inhibs_ini,
        InhibitorySyns, {});



    // Configure spike variables so that they can be downloaded to host
    e->setSpikeVarMode(VarMode::LOC_HOST_DEVICE_INIT_DEVICE);
    i->setSpikeVarMode(VarMode::LOC_HOST_DEVICE_INIT_DEVICE);

    model.finalize();
}
