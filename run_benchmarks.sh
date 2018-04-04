#!/bin/bash

auryn=true
spike=true
pyNest=true
brian=true
brian2genn=true
ANNarchy=true

options="--simtime 100 --num_timesteps_min_delay 8 --fast"

source deactivate
if [ "${auryn}" == true ] ; then
	#### AURYN TEST
	cd Benchmarks/VogelsAbbott/auryn
	make all
	./sim_coba_benchmark ${options}
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
	cd ../../../../
fi

source activate simulatorcomparison
if [ "${brian}" == true ] ; then
	cd Benchmarks/VogelsAbbott/brian2
	python COBA.py ${options}
	cd ../../../
fi


if [ "${brian2genn}" == true ] ; then
	cd Benchmarks/VogelsAbbott/brian2genn
	python COBA.py ${options}
	cd ../../../
fi


if [ "${ANNarchy}" == true ] ; then
	cd Benchmarks/VogelsAbbott/ANNarchy
	python COBA.py ${options}
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
	cd ../../../
fi


