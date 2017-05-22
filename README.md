# gas2017

## Video

10.000.000 million particles in a square domain. Central particles get a high initial velocity and crash into the rest of the material. (Domain is rotated by 90°)

[![Link to youtube video of simulation](https://img.youtube.com/vi/2xDZQ5mNiv8/0.jpg)](https://www.youtube.com/watch?v=2xDZQ5mNiv8)

## Download
    git clone https://github.com/hausler89/gas2017.git
    
## Description
Simple n-body simulation of particles with Lennard-Jones repulsion between them.
North and South wall are periodic, east and west are LJ repulsive.

Main source file is gas.cpp, there you will find the definition of global variables, system parameters and the main function. The verlet integrator is directly implemented in main(), as this is the main purpose of this program anyways.

## Prerequesites

Needs ncurses to be available on your system. On Ubuntu use

    sudo apt-get install libncurses-dev

## Run
Compile with

	make

Compile with OpenMP support

	make openmp

Compile with ncurses graphical output

	make gfx
	
Compile and run with

	make run
	
Clean with

	make clean
	
Recompile everything

	make new
	
Compile with debugging symbols

	make debug
