# GeNN has a two stage compilation process
# You must ensure that GeNN has been installed correctly

# First, allow the code generation;
genn-buildmodel.sh model.cc 

# Finally compile the example
make -j8

# In order to run the model;
# ./simulator --simtime 100.0 --fast
