#!/bin/bash
fast=false
delay_in_timesteps=1

auryn=false
spike=false
pyNest=false
brian=true
ANNarchy=false

options="--simtime 100 --num_timesteps_min_delay ${delay_in_timesteps}"
timingfolder="_results/timestep_${delay_in_timesteps}_delay/"
if [ "${fast}" == true ] ; then
  options="${options} --fast"
fi

source deactivate
if [ "${auryn}" == true ] ; then
  #### AURYN TEST
  cd Benchmarks/VogelsAbbott/auryn
  make all
  ./sim_coba_benchmark ${options}

  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/auryn.dat
  fi
  cd ../../../
fi


if [ "${spike}" == true ] ; then
  #### SPIKE TEST
  cd Benchmarks/VogelsAbbott/Spike
  mkdir -p Build
  cd Build
  cmake ../
  make VogelsAbbottNet -j8
  ./VogelsAbbottNet ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../../${timingfolder}
    cp ./timefile.dat ../../${timingfolder}/Spike.dat
  fi
  cd ../../../../
fi

source activate simulatorcomparison
if [ "${brian}" == true ] ; then
  cd Benchmarks/VogelsAbbott/brian2
  python COBA.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/brian2.dat
  fi
  cd ../../../
fi


if [ "${ANNarchy}" == true ] ; then
  cd Benchmarks/VogelsAbbott/ANNarchy
  python COBA.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/ANNarchy.dat
  fi
  cd ../../../
fi


if [ "${pyNest}" == true ] ; then
  cd Benchmarks/VogelsAbbott/pyNest
  mkdir -p Build
  cd Build

  cmake -DCMAKE_INSTALL_PREFIX:PATH=${PWD}/ ${PWD}/../../../../Simulators/nest-simulator
  make
  make install

  cd ../
  source Build/bin/nest_vars.sh
  python COBA.py ${options}
  if [ "${fast}" == true ] ; then
    mkdir -p ../${timingfolder}
    cp ./timefile.dat ../${timingfolder}/Nest.dat
  fi
  cd ../../../
fi


