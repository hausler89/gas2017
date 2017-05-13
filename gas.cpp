#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>

using namespace std;

// Type declarations
struct vec;
struct particle;
typedef float scalar;
typedef vector<particle> particle_list;

// System parameters
const size_t N = 100;
scalar box_cutoff = 1.1225;
scalar height = 6.;
scalar width = 10.;

scalar pot_size = pow(2, 1. / 6.);
scalar pot_size6 = 2;

scalar dt = 1e-3;

// Function declarations
void update_force(particle_list &);

// Definitions

// Basic 2D vector. Uses standard copy constructor
struct vec
{
	scalar x;
	scalar y;

	// Simple constructor initializing with zeroes
	vec()
	{
		x = 0;
		y = 0;
	}

	// Constructor with custom initializations
	vec(scalar ix, scalar iy)
	{
		x = ix;
		y = iy;
	}

	// Define the '+'-operator for vectors
	vec operator+(const vec &rhs)
	{
		return vec(x + rhs.x, y + rhs.y);
	}
	vec &operator+=(const vec &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
};

// Multiplication of vector with scalar
vec operator*(const scalar &lhs, const vec &rhs)
{
	return vec(lhs * rhs.x, lhs * rhs.y);
}

// A particle, containing position, velocity and a force acting on it
struct particle
{
	vec r;
	vec v;
	vec F;

	// Previous force, as temp variable needed for the verlet algorithm
	// Is written by the 'update_force' function.
	vec pF;
};

int main()
{
	srand(time(NULL));

	particle_list p(N);

	scalar T = 0;

	update_force(p);

	// Verlet integration
	while (T < 1)
	{
		for (auto i : p)
		{
			i.r += dt * i.v + 0.5 * dt * dt * i.F;
			i.r.y = fmod(i.r.y, height);
		}

		update_force(p);

		for (auto i : p)
			i.v += 0.5 * dt * (i.F + i.pF);

		T += dt;
	}
}

// Recalculate the forces acting on the particles.
// Will backup the previous force to the pV member of the particles.
void update_force(particle_list &p)
{
	// Backup the force and calculate the wall repulsion
	for (auto i : p)
	{
		i.pF = i.F;

		scalar d;

		bool within_reach = false;
		scalar force_direction = 0;

		if (i.r.x < pot_size)
		{
			d = i.r.x;
			within_reach = true;
			force_direction = 1;
		}
		else if (i.r.x > width - pot_size)
		{
			d = width - i.r.x;
			within_reach = true;
			force_direction = -1;
		}

		if (within_reach)
		{
			scalar d2 = d * d;
			scalar d6 = d2 * d2 * d2;
			scalar F_wall = 6 * pot_size6 * (d6 - 2 * pot_size6) / (d6 * d6 * d);

			i.F = vec(F_wall * force_direction, 0);
		}
		else
			i.F = vec(0, 0);
	}

	// Calculating the forces for all particle combinations
	for (size_t i = 0; i < p.size() - 1; ++i)
	{

		for (size_t j = i + 1; j < p.size(); ++j)

			if (abs(p[i].r.x - p[j].r.x) < box_cutoff && abs(p[i].r.y - p[j].r.y) < box_cutoff)
			{
			}
	}
}