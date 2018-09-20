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

#include <getopt.h>
#include <time.h>
#include <iomanip>

using namespace BoBRobotics;

int main (int argc, char *argv[])
{
    // Getting options:
    float simtime = 20.0;
    bool fast = false;
    const char* const short_opts = "";
    const option long_opts[] = {
      {"simtime", 1, nullptr, 0},
      {"fast", 0, nullptr, 1},
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
        default:
          break;
      }
    };
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
        ragged_connectivity_from_mat("../ee.wmat", gEE, CEE.ind, CEE.rowLength, Parameters::numExcitatory, Parameters::EEMaxRow);
        reset_array(inSynEE, Parameters::numExcitatory);
        pushEEStateToDevice();

        ragged_connectivity_from_mat("../ei.wmat", gEI, CEI.ind, CEI.rowLength, Parameters::numExcitatory, Parameters::EIMaxRow);
        reset_array(inSynEI, Parameters::numInhibitory);
        pushEIStateToDevice();

        ragged_connectivity_from_mat("../ii.wmat", gII, CII.ind, CII.rowLength, Parameters::numInhibitory, Parameters::IIMaxRow);
        reset_array(inSynII, Parameters::numInhibitory);
        pushIIStateToDevice();

        ragged_connectivity_from_mat("../ie.wmat", gIE, CIE.ind, CIE.rowLength, Parameters::numInhibitory, Parameters::IEMaxRow);
        reset_array(inSynIE, Parameters::numExcitatory);
        pushIEStateToDevice();
    }

    // Final setup
    {
        Timer<> t("Sparse init:");
        initva_benchmark();
    }

    // Open CSV output files
    GeNNUtils::SpikeCSVRecorderDelay spikes("spikes.csv", 3200, spkQuePtrE, glbSpkCntE, glbSpkE);

    clock_t totaltime;
    {
        Timer<> t("Simulation:");
        // Loop through timesteps
        int timesteps_per_second = 10000;
        clock_t starttime = clock();
        for(unsigned int t = 0; t < (int)(simtime*timesteps_per_second); t++)
        {
            // Simulate
#ifndef CPU_ONLY
            stepTimeGPU();

            if (!fast) pullECurrentSpikesFromDevice();
#else
            stepTimeCPU();
#endif

            if (!fast) spikes.record(t);
        }
        totaltime = clock() - starttime;
    }
    if ( fast ){
      std::ofstream timefile;
      timefile.open("timefile.dat");
      timefile << std::setprecision(10) << ((float)totaltime / CLOCKS_PER_SEC);
      timefile.close();
    }

    return 0;
}
