#!/bin/bash
fast=true
plastic=true
simulation_length_in_seconds=100

auryn=false
spike=false
ANNarchy=false
brian2=true
pyNest=false

#### Setting Up Options
options="--simtime ${simulation_length_in_seconds}"
timingfolder="_results/simulation_speed/"
if [ "${fast}" == true ] ; then
  options="${options} --fast"
fi

if [ "${plastic}" == true ] ; then
  timingfolder="${timingfolder}plastic/"
  options="${options} --plastic"
else
  timingfolder="${timingfolder}non_plastic/"
fi


#### Running Simulations
source deactivate
if [ "${auryn}" == true ] ; then
  #### AURYN TEST
  cd Benchmarks/Brunel/auryn
  make all
  ./sim_brunel2k_pl ${options}

  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/auryn.dat
  fi
  cd ../../../
fi


if [ "${spike}" == true ] ; then
  #### SPIKE TEST
  cd Benchmarks/Brunel/Spike
  mkdir -p Build
  cd Build
  cmake ../
  make Brunel10K -j8
  ./Brunel10K ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../../${timingfolder}
    cp ./timefile.dat ../../${timingfolder}/Spike.dat
  fi
  cd ../../../../
fi

source activate simulatorcomparison
if [ "${brian2}" == true ] ; then
  cd Benchmarks/Brunel/brian2
  python Brunel10K.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/brian2.dat
  fi
  cd ../../../
fi


if [ "${ANNarchy}" == true ] ; then
  cd Benchmarks/Brunel/ANNarchy
  python Brunel10K.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/ANNarchy.dat
  fi
  cd ../../../
fi


if [ "${pyNest}" == true ] ; then
  cd Benchmarks/Brunel/pyNest
  mkdir -p Build
  cd Build

  cmake -DCMAKE_INSTALL_PREFIX:PATH=${PWD}/ ${PWD}/../../../../Simulators/nest-simulator
  make
  make install

  cd ../
  source Build/bin/nest_vars.sh
  python Brunel10K.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/Nest.dat
  fi
  cd ../../../
fi


