# Make a Build directory
mkdir -p Build
cd ./Build

# Now run cmake init
cmake ../

# Finally compile the example
make Brunel10K -j8

# In order to run the model;
# Ensure you are in the Build folder, and run the compiled file
# ./VogelsAbbottNet --simtime 100.0 --fast
