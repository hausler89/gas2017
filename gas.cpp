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
const size_t N = 2;
scalar box_cutoff = 1.1225;
scalar height = 6.;
scalar width = 10.;

scalar velocity_max = 1;

scalar pot_size = pow(2, 1. / 6.);
scalar pot_size6 = 2;

scalar dt = 1e-2;

// Function declarations
void update_force(particle_list &);
scalar lennard_jones(scalar);

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

	void shout()
	{
		cout << "I'm a particle @ x = " << r.x << ", y = " << r.y << endl;
	}

	// Previous force, as temp variable needed for the verlet algorithm
	// Is written by the 'update_force' function.
	vec pF;
};

int main()
{
	srand(time(NULL));

	particle_list p(N);

	for (auto &i : p)
	{
		scalar r_x = (scalar)rand() / RAND_MAX;
		scalar r_y = (scalar)rand() / RAND_MAX;
		scalar r_v = velocity_max * (scalar)rand() / RAND_MAX;
		scalar r_phi = 2 * M_PI * (scalar)rand() / RAND_MAX;

		r_x = pot_size + r_x * (width - 2 * pot_size);
		r_y *= height;

		i.r = vec(r_x, r_y);
		i.v = vec(sin(r_phi) * r_v, cos(r_phi) * r_v);
	}

	scalar T = 0;

	p[0].r = vec(3, 0);
	p[0].v = vec(-3, 0);

	p[1].r = vec(6, 0);
	p[1].v = vec(-1, 0);

	update_force(p);

	// Verlet integration
	bool integrate = true;
	while (T < 3)
	{
		p[0].shout();
		for (auto &i : p)
		{
			i.r += dt * i.v + 0.5 * dt * dt * i.F;

			while (i.r.y < 0)
				i.r.y += height;
			while (i.r.y > height)
				i.r.y -= height;
		}

		update_force(p);

		for (auto &i : p)
			i.v += 0.5 * dt * (i.F + i.pF);

		T += dt;
		integrate = false;
	}
}

// Recalculate the forces acting on the particles.
// Will backup the previous force to the pV member of the particles.
void update_force(particle_list &p)
{
	// Backup the force and calculate the wall repulsion
	for (auto &i : p)
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
			scalar F_wall = lennard_jones(d);
			i.F = vec(-F_wall * force_direction, 0);
		}
		else
			i.F = vec(0, 0);
	}

	// Calculating the forces for all particle combinations
	for (size_t i = 0; i < p.size() - 1; ++i)
	{

		for (size_t j = i + 1; j < p.size(); ++j)
		{

			scalar deltax = abs(p[i].r.x - p[j].r.x);
			scalar deltay = abs(p[i].r.y - p[j].r.y);

			if (deltax < box_cutoff && deltay < box_cutoff)
			{
				scalar r = sqrt(deltax * deltax + deltay * deltay);
				scalar F = lennard_jones(r);

				scalar Fx = F * deltax / r;
				scalar Fy = F * deltay / r;

				p[i].F += vec(Fx, Fy);
				p[j].F += vec(-Fx, -Fy);
			}
		}
	}
}

inline scalar lennard_jones(scalar d)
{
	scalar d6 = d * d * d * d * d * d;
	return 6 * pot_size6 * (d6 - 2 * pot_size6) / (d6 * d6 * d);
}