#include "force.h"
#include "vec.h"
#include "common.h"
#include <cmath>
#include "Dispatcher.h"

using namespace std;
extern Dispatcher D;

// Recalculate the forces acting on the particles.
// Will backup the previous force to the pV member of the particles.
void update_force(particle_list &p, const vector<char> box)
{
	// Backup the force and calculate the wall repulsion
	// This loop will initialize the force!
	for (auto &i : p)
	{
		i.pF = i.F;

		// Distance to the nearest wall
		scalar d;

		// Force will only be applied if its within reach
		bool within_reach = false;

		// Direction of the force will be either +1 or -1
		scalar force_direction = 0;

		// Check if we're near enough a wall
		if (i.r.x < box_cutoff)
		{
			// Set distance
			d = i.r.x;

			// Activate force calculation
			within_reach = true;

			// Set direction of the force
			force_direction = 1;
		}
		else if (i.r.x > width - box_cutoff)
		{
			// Distance to wall must be a positive number
			d = width - i.r.x;
			within_reach = true;
			force_direction = -1;
		}

		// Calculate the force if we're near enough
		if (within_reach)
		{
			scalar F_wall = lennard_jones(d);

			// Force is always perpendicular to the wall
			i.F = vec(-F_wall * force_direction, 0);
		}
		// If no wall force is applied, init the force to zero
		else
			i.F = vec(0, 0);
	}

	// Reset the dispatcher to the beginning
	D.reset();

	do
	{
		while (D.jobs_available())
		{
			// Get a new job
			job J = D.get_next_job();

			// Index of first particle (origin particle)
			int i1;

			// Index of particle that is compared to the origin particle
			int i2;

			// Get the first i1 particle (origin)
			i1 = next_origin(-1, box, J);

			// Simulation runs as long as there are particles in the central
			// box. Check wether we have a particle at all.
			bool origins_left = true;

			if (i1 < 0)
				origins_left = false;

			while (origins_left)
			{
				// Get the next particle
				i2 = next_particle(-1, box, J);

				// Get the next origin particle alread, so we can prefetch while doing the calculation
				int i1_next = next_origin(i1, box, J);

				bool particles_left = true;
				if (i2 < 0)
					particles_left = false;

				while (particles_left)
				{
					// Check if this is another origin particle. If it is,
					// then only calculate the force if we didn't do this before
					// and only if this is not the i1 itself.

					bool calculate = true;

					if (box[i2] == J.origin && i2 <= i1)
						calculate = false;

					// Get the next particle already, so we can prefetch the memory before
					// doing the force calculation
					int i2_next = next_particle(i2, box, J);

					if (calculate)
					{
						// Displacement "vector" from p[i] to p[j]
						scalar deltax = p[i1].r.x - p[i2].r.x;
						scalar deltay = p[i1].r.y - p[i2].r.y;

						// Periodic boundaries on north and south wall
						// Check if the distance to a parallel transported "copy"
						// of the second planet is shorter. We will only calculate
						// the force to ONE SINGULAR version of the particle, assuming
						// that the potential is always smaller than the domain
						if (abs(p[i1].r.y - p[i2].r.y - height) < abs(deltay))
							deltay = p[i1].r.y - p[i2].r.y - height;
						else if (abs(p[i1].r.y - p[i2].r.y + height) < abs(deltay))
							deltay = p[i1].r.y - p[i2].r.y + height;

						// Make a numerical cheap check if the particles might be able
						// to interact at all
						if (abs(deltax) < box_cutoff && abs(deltay) < box_cutoff)
						{
							// The distance between the particles
							scalar r = sqrt(deltax * deltax + deltay * deltay);

							// Magnitude of the force
							scalar F = lennard_jones(r);

							// Project the force onto the x and y direction
							scalar Fx = F * deltax / r;
							scalar Fy = F * deltay / r;

							// Add the forces to the two planets
							p[i1].F += vec(-Fx, -Fy);
							p[i2].F += vec(+Fx, +Fy);
						}
					} // End of if(calculate)

					// Set i2 to the next particle and check wether it exists
					if (i2_next < 0)
						particles_left = false;
					else
						i2 = i2_next;

				} // End of while(particles_left)

				// Set i1 to the next particle and check wether it exists
				if (i1_next < 0)
					origins_left = false;
				else
					i1 = i1_next;

			} // End of while(origins_left)

		} // End of while(D.jobs_available())

	} while (D.advance_phase());
}

// A simple Lennard-Jones force, calculated by the distance parameter only
// Strength is supplied by global variables. The force is cut off at a
// certain distance.
inline scalar lennard_jones(scalar d)
{
	// Cutoff distance
	if (d < pot_size)
	{
		// Sixth power of the distance
		scalar d6 = d * d * d * d * d * d;
		return 6 * pot_size6 * (d6 - 2 * pot_size6) / (d6 * d6 * d);
	}
	else
		return 0;
}

int next_origin(int i0, const vector<char> &box, job J)
{

	for (int i = i0 + 1; i < int(box.size()); ++i)
		if (box[i] == J.origin)
			return i;

	return -1;
}

int next_particle(int i0, const vector<char> &box, job J)
{
	for (int i = i0 + 1; i < int(box.size()); ++i)
	{
		if (box[i] == J.origin)
			return i;
		for (int k = 0; k < int(J.id.size()); ++k)
			if (box[i] == J.id[k])
				return i;
	}
	return -1;
}