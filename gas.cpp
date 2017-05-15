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

using namespace std;

// System parameters
extern const size_t N = 100;
extern const scalar box_cutoff = 1.1225;
extern const scalar pot_size = 1 * pow(2, 1. / 6.);
extern const scalar pot_size6 = 2;
extern const scalar height = 6.;
extern const scalar width = 10.;

extern const int grid_h = 9;
extern const int grid_w = 12;

extern const scalar velocity_max = 100;
extern const scalar dt = 1e-4;

// Function declarations
void update_force(particle_list &);
inline scalar lennard_jones(scalar);
void draw_particles(const particle_list &);

int main()
{
	init_gui();

	srand(time(NULL));

	particle_list p(N);

	ofstream file;
	file.open("output.dat");

	for (size_t i = 0; i < N; ++i)
	{
		scalar r_v = velocity_max * (scalar)rand() / RAND_MAX;
		scalar r_phi = 2 * M_PI * (scalar)rand() / RAND_MAX;

		int pos_x = i % grid_w;
		int pos_y = i / grid_w;

		scalar x = scalar(pos_x) / scalar(grid_w) * (width - 2 * pot_size) + pot_size;
		scalar y = scalar(pos_y) / scalar(grid_h + 1) * height;

		p[i].r = vec(x, y);
		p[i].v = vec(sin(r_phi) * r_v, cos(r_phi) * r_v);
	}

	scalar T = 0;
	scalar T_diag = 0;

	update_force(p);

	// Verlet integration
	try
	{
		while (T < 20)
		{

			if (T_diag > 1 * dt)
			{
				T_diag = 0;

				draw_particles(p);
				refresh();
				usleep(10000);
			}

			//limit_force(p, 0.5 * 2 * pot_size / (dt * dt));
			//limit_velocity(p, 0.5 * pot_size / dt);

			for (auto &i : p)
			{
				i.r += dt * i.v + 0.5 * dt * dt * i.F;

				if (isnan(i.r.y) || isnan(i.r.x))
					throw 100; // Error code for NaN

				while (i.r.y < 0)
				{
					i.r.y += height;
				}
				while (i.r.y > height)
				{
					i.r.y -= height;
				}

				if (i.r.x > width || i.r.x < 0)
					throw 200; // Error code for leaving the area
			}

			update_force(p);

			for (auto &i : p)
				i.v += 0.5 * dt * (i.F + i.pF);

			T += dt;
			T_diag += dt;
		}
	}
	catch (int e)
	{
		switch (e)
		{
		case 100:
			cout << "NaN in variable occured" << endl;
		case 200:
			cout << "Particle left boundary" << endl;
		}
	}
	endwin();
}
