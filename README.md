# gas2017

## Download
    git clone https://github.com/hausler89/gas2017.git
    
## Description
Simple n-body simulation of particles with Lennard-Jones repulsion between them.
North and South wall are periodic, east and west are LJ repulsive.

Main source file is gas.cpp, there you will find the definition of global variables, system parameters and the main function. The verlet integrator is directly implemented in main(), as this is the main purpose of this program anyways.

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
