#pragma once

// GeNN includes
#include "modelSpec.h"

namespace BoBRobotics {
namespace GeNNModels {

//----------------------------------------------------------------------------
// BoBRobotics::GeNNModels::LIF
//----------------------------------------------------------------------------
//! Leaky integrate-and-fire neuron solved algebraically
class LIF : public NeuronModels::Base
{
public:
    DECLARE_MODEL(LIF, 7, 2);

    SET_SIM_CODE(
        "if ($(RefracTime) <= 0.0)\n"
        "{\n"
        "  scalar alpha = (($(Isyn)) * $(Rmembrane));\n"
        "  $(V) += (DT / $(TauM))*(($(Vrest) - $(V)) + alpha + $(Ioffset));\n"
        "}\n"
        "else\n"
        "{\n"
        "  $(RefracTime) -= DT;\n"
        "}\n"
    );

    SET_THRESHOLD_CONDITION_CODE("$(RefracTime) <= 0.0 && $(V) >= $(Vthresh)");

    SET_RESET_CODE(
        "$(V) = $(Vreset);\n"
        "$(RefracTime) = $(TauRefrac);\n");

    SET_PARAM_NAMES({
        "C",          // Membrane capacitance
        "TauM",       // Membrane time constant [ms]
        "Vrest",      // Resting membrane potential [mV]
        "Vreset",     // Reset voltage [mV]
        "Vthresh",    // Spiking threshold [mV]
        "Ioffset",    // Offset current
        "TauRefrac"});

    SET_DERIVED_PARAMS({
        {"ExpTC", [](const vector<double> &pars, double dt){ return std::exp(-dt / pars[1]); }},
        {"Rmembrane", [](const vector<double> &pars, double){ return  pars[1] / pars[0]; }}});

    SET_VARS({{"V", "scalar"}, {"RefracTime", "scalar"}});
};
IMPLEMENT_MODEL(LIF);
} // GeNNModels
} // BoBRobotics

