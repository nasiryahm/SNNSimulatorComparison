#!/bin/bash

auryn=true
spike=false
nest=false
brian=false
brian2genn=false
ANNarchy=false

if [ "${auryn}" == true ] ; then
	#### AURYN TEST
	cd Benchmarks/VogelsAbbott/auryn
	make all
	time ./sim_coba_benchmark --simtime 100 --fast
fi


if [ "${spike}" == true ] ; then
	#### SPIKE TEST
	cd Benchmarks/VogelsAbbott/Spike
	mkdir -p Build
	cd Build
	cmake ../
	make VogelsAbbottNet -j8
	time ./VogelsAbbottNet --simtime 100
fi


if [ "${nest}" == true ] ; then
	cd Benchmarks/VogelsAbbott/nest-simulator/
	mkdir -p Build
	cd Build

	cmake -DCMAKE_INSTALL_PREFIX:PATH=${PWD}/ ${PWD}/../../../../Simulators/nest-simulator
	make
	make install

	cd ../
	time Build/bin/nest coba.sli
fi


if [ "${brian}" == true ] ; then
	cd Benchmarks/VogelsAbbott/brian2
	time python COBA.py --fast --simtime 100
fi

if [ "${brian2genn}" == true ] ; then
	cd Benchmarks/VogelsAbbott/brian2genn
	time python COBA.py --fast --simtime 100
fi

if [ "${ANNarchy}" == true ] ; then
	cd Benchmarks/VogelsAbbott/ANNarchy
	time python COBA.py --fast --simtime 100
fi
