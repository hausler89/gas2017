#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <curses.h>
#include <unistd.h>
#include "common.h"
#include "vec.h"
#include "particle.h"
#include "gui.h"
#include "force.h"

using namespace std;

// SYSTEM PARAMETERS

// Particle count
extern const size_t N = 100;

// Step size for integration
extern const scalar dt = 1e-4;

// Maximum distance for force calculation
extern const scalar box_cutoff = 1.1225;

// Range parameter for Lennard-Jones-Potential
extern const scalar pot_size = 1 * pow(2, 1. / 6.);
extern const scalar pot_size6 = 2; // pot_size^6

// Domain size
extern const scalar height = 6.;
extern const scalar width = 10.;

// Grid of initial positions
extern const int grid_h = 9;
extern const int grid_w = 12;

// Maximum initial velocity
extern const scalar velocity_max = 100;


// Function declarations

int main()
{
	// Initialize ncurses window
	init_gui();

	// Seed the RNG
	srand(time(NULL));

	// Create a list of particles
	// particle_list is a vector of particles
	particle_list p(N);

	// Init the particles
	for (size_t i = 0; i < N; ++i)
	{
		// Velocity randomized, random speed and direction
		scalar r_v = velocity_max * (scalar)rand() / RAND_MAX;
		scalar r_phi = 2 * M_PI * (scalar)rand() / RAND_MAX;

		// Set the random velocity
		p[i].v = vec(sin(r_phi) * r_v, cos(r_phi) * r_v);

		// Determine position on the grid
		int pos_x = i % grid_w;
		int pos_y = i / grid_w;

		// Convert grid postion to physical position
		// We stay away from the repulsive walls (east and west) to
		// not introduce more energy to the system
		scalar x = scalar(pos_x) / scalar(grid_w) * (width - 2 * pot_size) + pot_size;
		scalar y = scalar(pos_y) / scalar(grid_h + 1) * height;

		// Set position
		p[i].r = vec(x, y);
	}

	// Update the force once, so that the first verlet step
	// has something to work with
	update_force(p);

	// Physical time, increased by the simulation loop
	scalar T = 0;

	// Diagnostic time
	// After this timer reached a certain value,
	// diagnostic information or screen redraws will be issue
	// and the timer will be reset.
	scalar T_diag = 0;

	// #### VERLET INTEGRATION ####
	// We wrap the integration into a try catch block so we can throw some
	// error codes.
	// Currently checked:
	//				- particle tunneling through west or east walls
	//				- NaN values in particle position
	try
	{
		// Integrate until the system reaches a desired time
		while (T < 20)
		{
			// Is it time for a screen refresh again?
			if (T_diag > 1 * dt)
			{
				// Reset diagnostic timer
				T_diag = 0;

				// Draw the particles to the screen
				draw_particles(p);

				// Update the screen
				refresh();

				// Wait for a little bit, to keep fps to peasant levels
				usleep(10000);
			}

			// Step 1: Update all particle positions (drift)
			for (auto &i : p)
			{
				// Drift
				i.r += dt * i.v + 0.5 * dt * dt * i.F;

				// Test for NaN in the position
				if (isnan(i.r.y) || isnan(i.r.x))
					throw 100; // Error code for NaN

				// Periodic boundary: Move the particle back
				// to the simulation domain. This can not handle
				// particles that move multiple domain heights in
				// one step (although that would probably break the
				// simulation anyways)
				if (i.r.y < 0)
					i.r.y += height;
				else if (i.r.y > height)
					i.r.y -= height;

				// Check if the particle left the domain through the
				// east or west boundary
				if (i.r.x > width || i.r.x < 0)
					throw 200; // Error code for leaving the area
			}

			// Step 2: Update particle forces
			update_force(p);

			// Step 3: Update the particles' velocities (kick)
			// pF denotes the force from the last step, prior
			// to the force update
			for (auto &i : p)
				i.v += 0.5 * dt * (i.F + i.pF);

			// Update the timers
			T += dt;
			T_diag += dt;
		}
	}

	// Catch thrown errors and inform the user about what happened.
	catch (int e)
	{
		switch (e)
		{
		case 100:
		{
			endwin();
			cout << "NaN in variable occured" << endl;
		}
		case 200:
		{
			endwin();
			cout << "Particle left boundary" << endl;
		}
		}
	}

	// Terminate the curses window
	endwin();
}
