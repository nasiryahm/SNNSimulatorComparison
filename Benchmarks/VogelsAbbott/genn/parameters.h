#pragma once

// Standard C includes
#include <cmath>

//------------------------------------------------------------------------
// Parameters
//------------------------------------------------------------------------
namespace Parameters
{
    const double timestep = 1.0;

    // number of cells
    const unsigned int numNeurons = 4000;

    const double resetVoltage = -60.0;
    const double restVoltage = -60.0;
    const double thresholdVoltage = -50.0;

    // connection probability
    const double probabilityConnection = 0.02;

    // number of excitatory cells:number of inhibitory cells
    const double excitatoryInhibitoryRatio = 4.0;

    const unsigned int numExcitatory = (unsigned int)std::round(((double)numNeurons * excitatoryInhibitoryRatio) / (1.0 + excitatoryInhibitoryRatio));
    const unsigned int numInhibitory = numNeurons - numExcitatory;

    const unsigned int EEMaxRow = 95;
    const unsigned int EIMaxRow = 31;
    const unsigned int IIMaxRow = 31;
    const unsigned int IEMaxRow = 87;

    const double scale = (4000.0 / (double)numNeurons) * (0.02 / probabilityConnection);

    const double excitatoryWeight = 0.4f; // * scale;
    const double inhibitoryWeight = 5.1f; // * scale;

}
