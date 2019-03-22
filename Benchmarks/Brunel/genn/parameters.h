#pragma once

// Standard C includes
#include <cmath>

//------------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------------
namespace Parameters
{
    const double timestep = 0.1;

    // number of cells
    const unsigned int numNeurons = 10000;

    const double resetVoltage = 0.0;
    const double restVoltage = 0.0;
    const double thresholdVoltage = 20.0;

    // connection probability
    const double probabilityConnection = 0.1;

    // number of excitatory cells:number of inhibitory cells
    const double excitatoryInhibitoryRatio = 4.0;

    const unsigned int numPoisson = numNeurons;
    const unsigned int numExcitatory = (unsigned int)std::round(((double)numNeurons * excitatoryInhibitoryRatio) / (1.0 + excitatoryInhibitoryRatio));
    const unsigned int numInhibitory = numNeurons - numExcitatory;

    const unsigned int EEMaxRow = 884;
    const unsigned int EIMaxRow = 256;
    const unsigned int IIMaxRow = 247;
    const unsigned int IEMaxRow = 893;

    const unsigned int synapticDelay = 15;

    const double scale = (4000.0 / (double)numNeurons) * (0.02 / probabilityConnection);

    const double excitatoryWeight = 0.1; // Plus conversion to amps
    const double inhibitoryWeight = -5.0f*excitatoryWeight; //

}
