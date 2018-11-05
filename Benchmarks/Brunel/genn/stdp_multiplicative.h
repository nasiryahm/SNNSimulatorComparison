#pragma once

// GeNN includes
#include "modelSpec.h"

//----------------------------------------------------------------------------
// STDPAdditive
//----------------------------------------------------------------------------
class STDPWeightDependent : public WeightUpdateModels::Base
{
public:
    DECLARE_MODEL(STDPWeightDependent, 8, 5);

    SET_PARAM_NAMES({
      "tauPlus",  // 0 - Potentiation time constant (ms)
      "tauMinus", // 1 - Depression time constant (ms)
      "Aplus",    // 2 - Rate of potentiation
      "Aminus",   // 3 - Rate of depression
      "Wmin",     // 4 - Minimum weight
      "Wmax",     // 5 - Maximum weight
      "lambda",   // 6 - Learning Rate
      "alpha",    // 7 - Relative Weighting (LTP to LTD)
    });

    SET_VARS({
      {"g", "scalar"},
      {"pre_trace", "scalar"},
      {"t_preUpdate", "scalar"},
      {"post_trace", "scalar"},
      {"t_postUpdate", "scalar"},
  });

    /*
    SET_SIM_CODE(
        "$(addtoinSyn) = $(g);\n"
        "$(updatelinsyn);\n"
        //"scalar dt = $(t) - $(sT_post); \n"
        //"if (fabs(dt) < DT)\n"
        //"{\n"
        // Decay Pre and Add Aplus
        "    scalar predt = $(t) - $(t_preUpdate)\n;"
        "    $(t_preUpdate) = $(t);\n"
        "    scalar preDecay = exp(- predt / $(tauPlus));\n"
        "    $(pre_trace) *= preDecay;\n"
        "    $(pre_trace) += $(Aplus);\n" 

        // Decay Post and Carry out update
        "    scalar decayamount = exp(- ($(t) - $(t_postUpdate)) / $(tauMinus));\n"
        "    $(t_postUpdate) = $(t);\n"
        "    $(post_trace) *= decayamount;\n"
        "    scalar newWeight = $(g) - $(lambda)*$(alpha)*$(g)*$(post_trace);\n"
        "    $(g) = (newWeight < $(Wmin)) ? $(Wmin) : newWeight;\n"
        //"}\n"
      );
    SET_LEARN_POST_CODE(
        //"scalar dt = $(t) - $(sT_pre);\n"
        //"scalar dt = $(t) - $(sT_pre); \n"
        //"if (fabs(dt) < DT)\n"
        //"{\n"
        // Decay Post and Add Aminus
        "    scalar postDecay = exp(- ($(t) - $(t_postUpdate)) / $(tauMinus));\n"
        "    $(t_postUpdate) = $(t);\n"
        "    $(post_trace) *= postDecay;\n"
        "    $(post_trace) += $(Aminus);\n" 

        // Decay pre and modify weight
        "    scalar decayamount = exp(- ($(t) - $(t_preUpdate)) / $(tauPlus));\n"
        "    $(pre_trace) *= decayamount;\n"
        "    $(t_preUpdate) = $(t);\n"
        "    scalar newWeight = $(g) + $(lambda)*($(Wmax) - $(g))*$(pre_trace);\n"
        "    $(g) = (newWeight > $(Wmax)) ? $(Wmax) : newWeight;\n"
        //"}\n"
        );
        */
    SET_SIM_CODE(
        "$(addtoinSyn) = $(g);\n"
        "$(updatelinsyn);\n"
        )
    SET_SYNAPSE_DYNAMICS_CODE(
        "scalar postDecay = exp(- DT / $(tauMinus));\n"
        "$(post_trace) *= postDecay;\n"
        "scalar preDecay = exp(- DT / $(tauPlus));\n"
        "$(pre_trace) *= preDecay;\n"
        
        "scalar post_dt = ($(t) - $(sT_post)); \n"
        "scalar pre_dt = ($(t) - $(sT_pre)); \n"
        "if (fabs(post_dt) < 0.5f*DT)\n"
        //"printf(\"%f -- %f, %f\\n\", $(t), $(sT_pre), $(sT_post));\n"
        "{\n"
        "    $(post_trace) += $(Aminus);\n" 
        "}\n"
        "if (fabs(pre_dt) < 0.5f*DT)\n"
        "{\n"
        "    $(pre_trace) += $(Aplus);\n" 
        "}\n"
        
        "scalar newWeight = $(g);\n"
        "scalar oldWeight = $(g);\n"
        "if (fabs(post_dt) < 0.5f*DT)\n"
        "{\n"
        "    newWeight += $(lambda)*($(Wmax) - oldWeight)*$(pre_trace);\n"
        //"printf(\"-- %f\\n\", $(sT_post));\n"
        "    newWeight = (newWeight > $(Wmax)) ? $(Wmax) : newWeight;\n"
        "}\n"
        "if (fabs(pre_dt) < 0.5f*DT)\n"
        "{\n"
        "    newWeight -= $(lambda)*$(alpha)*oldWeight*$(post_trace);\n"
        "    newWeight = (newWeight < $(Wmin)) ? $(Wmin) : newWeight;\n"
        "}\n"
        "$(g) = newWeight;\n"
        );

    SET_NEEDS_PRE_SPIKE_TIME(true);
    SET_NEEDS_POST_SPIKE_TIME(true);
};

IMPLEMENT_MODEL(STDPWeightDependent);
