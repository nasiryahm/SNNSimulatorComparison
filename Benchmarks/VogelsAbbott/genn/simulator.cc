// Standard C++ includes
#include <random>

// GeNN robotics includes
//#include "common/timer.h"
//#include "genn_utils/spike_csv_recorder.h"
#include "timer.h"
#include "spike_csv_recorder.h"

// Model parameters
#include "parameters.h"

// Connectivity functions
#include "matLoader.h"

// Auto-generated model code
#include "va_benchmark_CODE/definitions.h"

using namespace BoBRobotics;

int main()
{
    {
        Timer<> t("Allocation:");
        allocateMem();
    }
    
    {
        Timer<> t("Initialization:");
        initialize();
    }
    
    // Loading Synapses
    {
        Timer<> t("Synapse setup:");
        ragged_connectivity_from_mat("../ee.wmat", CEE.ind, CEE.rowLength, Parameters::numExcitatory, Parameters::EEMaxRow);
        pushEEStateToDevice();
        ragged_connectivity_from_mat("../ei.wmat", CEI.ind, CEI.rowLength, Parameters::numExcitatory, Parameters::EIMaxRow);
        pushEIStateToDevice();
        ragged_connectivity_from_mat("../ii.wmat", CII.ind, CII.rowLength, Parameters::numInhibitory, Parameters::IIMaxRow);
        pushIIStateToDevice();
        ragged_connectivity_from_mat("../ie.wmat", CIE.ind, CIE.rowLength, Parameters::numInhibitory, Parameters::IEMaxRow);
        pushIEStateToDevice();
    }

    // Final setup
    {
        Timer<> t("Sparse init:");
        initva_benchmark();
    }

    // Open CSV output files
    GeNNUtils::SpikeCSVRecorder spikes("spikes.csv", glbSpkCntE, glbSpkE);

    {
        Timer<> t("Simulation:");
        // Loop through timesteps
        int timesteps_per_second = 10000;
        for(unsigned int t = 0; t < 100*timesteps_per_second; t++)
        {
            // Simulate
#ifndef CPU_ONLY
            stepTimeGPU();

            pullECurrentSpikesFromDevice();
#else
            stepTimeCPU();
#endif

            spikes.record(t);
        }
    }

    return 0;
}
