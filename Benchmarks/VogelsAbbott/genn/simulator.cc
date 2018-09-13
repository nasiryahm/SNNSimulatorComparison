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
    
    // Final setup
    {
        Timer<> t("Synapse setup:");
        connectivity_from_mat("../ee.wmat", gEE, Parameters::numExcitatory, Parameters::numExcitatory);
        connectivity_from_mat("../ei.wmat", d_gEI, Parameters::numExcitatory, Parameters::numInhibitory);
        connectivity_from_mat("../ii.wmat", d_gII, Parameters::numInhibitory, Parameters::numInhibitory);
        connectivity_from_mat("../ie.wmat", d_gIE, Parameters::numInhibitory, Parameters::numExcitatory);
    }
    {
        Timer<> t("Initialization:");
        initialize();
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
        for(unsigned int t = 0; t < 1*timesteps_per_second; t++)
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
