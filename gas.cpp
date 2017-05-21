#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <curses.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include "vec.h"
#include "particle.h"
#include "gui.h"
#include "force.h"
#include "Dispatcher.h"
#include "dispatch.h"
#include "common.h"

using namespace std;

// COMPILE TIME FLAGS

// if USE_GUI is defined, the domain will be rendered with
// ncurses. Comment this line to disable this feature.
// Compiling with 'make gfx' will also set the variable.
// #define USE_GUI

// SYSTEM PARAMETERS

// Particle count
extern const size_t N = 10000000;

// Step size for integration
extern const scalar dt = 1e-4;

// Information or screen refreshes come in these intervals
#ifdef USE_GUI
const scalar T_diag_max = 10 * dt;
#else
const scalar T_diag_max = 100 * dt;
#endif

// Maximum distance for force calculation
// extern const scalar box_cutoff = 1.1225;
extern const scalar box_cutoff = 2;

// Range parameter for Lennard-Jones-Potential
extern const scalar pot_size = 1 * pow(2, 1. / 6.);
extern const scalar pot_size6 = 2; // pot_size^6

// Domain size
extern const scalar height = 2700;
extern const scalar width = 2700;

// Grid of initial positions
extern const int grid_h = 3170;
extern const int grid_w = 3170;

// Maximum initial velocity
extern const scalar velocity_max = 10;

// Calculation box count (for parallelism)
extern const int num_boxes_x = int(width / box_cutoff) + 1;
extern const int num_boxes_y = int(height / box_cutoff) + 1;

extern const int num_boxes = num_boxes_x * num_boxes_y;

Dispatcher D;

int main()
{
	D = Dispatcher();

#ifdef USE_GUI
	// Initialize ncurses window
	init_gui();
#endif

	// cout << "boxes: " << num_boxes_x * num_boxes_y << endl;
	// do
	// {
	// 	while (D.jobs_available())
	// 	{
	// 		job J = D.get_next_job();
	// 		cout << int(J.origin) << endl;
	// 	}
	// } while (D.advance_phase());

	// return 0;

	// Seed the RNG
	srand(time(NULL));

	// Create a list of particles
	// particle_list is a vector of particles
	particle_list p(N);

	// Create a list of ids that indicates in which box a particle is in.
	vector<vector<int>> box(num_boxes);

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

		scalar cx = width / 2.;
		scalar cy = height / 2.;

		scalar deltax = abs(cx - x);
		scalar deltay = abs(cy - y);

		scalar r = sqrt(deltax * deltax + deltay * deltay);

		if (r < width / 10.)
			p[i].v = vec(0, 200);

		int id = coord2id(x, y);

		box[id].push_back(i);
	}

	// Update the force once, so that the first verlet step
	// has something to work with
	update_force(p, box);

	// Physical time, increased by the simulation loop
	scalar T = 0;

	// Diagnostic time
	// After this timer reached a certain value (T_diag_max),
	// diagnostic information or screen redraws will be issued
	// and the timer will be reset.
	scalar T_diag = 0;

	// #### VERLET INTEGRATION ####
	// We wrap the integration into a try catch block so we can throw some
	// error codes.
	// Currently checked:
	//				- particle tunneling through west or east walls
	//				- NaN values in particle position
	int out_count = 0;
	try
	{
		// Integrate until the system reaches a desired time
		while (T < 100)
		{
			// Is it time for a screen refresh again?
			if (T_diag > T_diag_max)
			{
				// Reset diagnostic timer
				T_diag = 0;
				stringstream ss;

				ss << "out/dump" << setfill('0') << setw(6) << out_count << ".dat";
				ofstream FILE;
				FILE.open(ss.str().c_str(), std::ofstream::out);
				for (int i = 0; i < num_boxes; ++i)
					FILE << box[i].size() << endl;
				FILE.close();

				out_count++;

#ifdef USE_GUI
				// Draw the particles to the screen
				draw_particles(p);

				// Update the screen
				refresh();

				// Wait for a little bit, to keep fps to peasant levels
				usleep(100000);
#endif

#ifndef USE_GUI
				// Output current time to the terminal
				cout << "simulation time: " << T << endl;
#endif
			}

			// Remove old ids
			// #pragma omp parallel for
			for (int i = 0; i < num_boxes; ++i)
				box[i].clear();

			// Step 1: Update all particle positions (drift)
			// #pragma omp parallel for
			for (size_t part = 0; part < p.size(); ++part)
			{
				particle &i = p[part];
				// Drift
				i.r += dt * i.v + 0.5 * dt * dt * i.F;

				// Update the boxes
				// #pragma omp critical
				{
					box[coord2id(i.r.x, i.r.y)].push_back(part);
				}
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
			update_force(p, box);

			// Step 3: Update the particles' velocities (kick)
			// pF denotes the force from the last step, prior
			// to the force update
			// #pragma omp parallel for
			for (size_t part = 0; part < p.size(); ++part)
			{
				particle &i = p[part];
				i.v += 0.5 * dt * (i.F + i.pF);
			}
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
#ifdef USE_GUI
	// Terminate the curses window
	endwin();
#endif
}