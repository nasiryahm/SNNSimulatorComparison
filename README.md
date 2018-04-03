# SNNSimulatorComparison
Comparison of Spiking Neural Network Simulator Performance

Simulators are included as git submodules.
Currently the simulators being compared are:
- NEST Simulator
- Brian Simulator (v2)
- Auryn
- ANNarchy
- Spike 
- Brian2 with GeNN (General Neural Network Simulator)

## Installation
Spike, Auryn and Nest are auto compiled (using make). Ensure that the dependencies for these libraries are pre-installed. To see these, please visit the github pages for these projects.

ANNarchy and Brian can be installed using 'pip install Simulators/ANNarchy/' and 'pip install Simulators/brian2' from this location. Again, ensure that the dependencies are pre-installed.
The dependencies and packages required for the installation of these two packages can be found in the requirements.txt file in the root of this repo.


Brian with GeNN must be installed separately. If using conda, 'conda install -c brian-team brian2genn'. Otherwise, you must set the CUDA\_PATH and GENN\_PATH variables and add ${GENN\_PATH}/lib/bin to PATH. Following this, it can be installed as above with pip: 'pip instgall Simulators/brian2genn'

## Testing ranges of delays:
Spike, Brian2, and Nest support ranges of delays. ANNarchy informs us that it can handle uniform delays (experimentally) though does not compile in this case.

Auryn and Brian2GeNN do not seem to support ranges of delays.
