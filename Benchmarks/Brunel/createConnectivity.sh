#!/bin/bash

# Script to create the connectivity for the Brunel Network
# Before running this, ensure that auryn has been installed in the Simulator/auryn directory

echo "Building a Connectivity Matrices Using Auryn"

# Building the Model
cd ./auryn
./compile.sh

# Running the Model with our required settings
./sim_brunel2k_pl --save "network" --simtime 0 --dir ./

# Now copy the corresponding files to the correct locations
mv network.0.0.wmat ../ee.wmat
mv network.1.0.wmat ../ei.wmat
mv network.2.0.wmat ../ii.wmat
mv network.3.0.wmat ../ie.wmat

echo "Connectivity Creation Complete!"


