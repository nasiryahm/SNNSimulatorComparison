#!/bin/bash
fast=false
delay_in_seconds=15
simulation_length_in_seconds=100
timestep=0.0001

auryn=true
spike=false
pyNest=false
brian2=false
ANNarchy=false

options="--simtime ${simulation_length_in_seconds} --min_delay ${delay_in_seconds} --timestep ${timestep}"
timingfolder="_results/timing/"
if [ "${fast}" == true ] ; then
	options="${options} --fast"
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
	./VogelsAbbottNet ${options}
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


