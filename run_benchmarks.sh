#!/bin/bash

auryn=false
spike=false
nest=false
brian=true

if [ "$auryn" == true ] ; then
	#### AURYN TEST
	cd Benchmarks/VogelsAbbott/auryn
	make all
	time ./sim_coba_benchmark --simtime 60 --fast
fi


if [ "$spike" == true ] ; then
	#### SPIKE TEST
	cd Benchmarks/VogelsAbbott/Spike
	mkdir -p Build
	cd Build
	cmake ../
	make VogelsAbbottNet -j8
	time ./VogelsAbbottNet --simtime 60 --fast
fi


if [ "$nest" == true ] ; then
	cd Benchmarks/VogelsAbbott/nest-simulator/
	mkdir -p Build
	cd Build

	cmake -DCMAKE_INSTALL_PREFIX:PATH=${PWD}/ ${PWD}/../../../../Simulators/nest-simulator
	make
	make install

	cd ../
	time Build/bin/nest coba.sli
fi


if [ "$brian" == true ] ; then
	cd Benchmarks/VogelsAbbott/brian2
	time python CUBA.py --fast --simtime 100
fi
