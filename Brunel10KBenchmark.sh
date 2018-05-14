#!/bin/bash
fast=true
plastic=true
delay_in_timesteps=15 # Assuming 0.1ms timestep
simulation_length_in_seconds=100

auryn=false
spike=false
ANNarchy=true
brian2=false
pyNest=false

options="--simtime ${simulation_length_in_seconds} --num_timesteps_min_delay ${delay_in_timesteps}"
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

source deactivate
if [ "${auryn}" == true ] ; then
  #### AURYN TEST
  cd Benchmarks/Brunel10K/auryn
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
  cd Benchmarks/Brunel10K/Spike
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
  cd Benchmarks/Brunel10K/brian2
  python Brunel10K.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/brian2.dat
  fi
  cd ../../../
fi


if [ "${ANNarchy}" == true ] ; then
  cd Benchmarks/Brunel10K/ANNarchy
  python Brunel10K.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/ANNarchy.dat
  fi
  cd ../../../
fi


if [ "${pyNest}" == true ] ; then
  cd Benchmarks/Brunel10K/pyNest
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


