#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

// Type declarations
struct vec;
struct particle;
typedef double scalar;
typedef vector<particle> particle_list;

// System parameters
const size_t N = 100;
const scalar box_cutoff = 1.1225;
const scalar pot_size = pow(2, 1. / 6.);
const scalar height = 6.;
const scalar width = 10.;

const int grid_h = 9;
const int grid_w = 12;

const scalar velocity_max = 1;

const scalar pot_size6 = 2;

const scalar dt = 1e-4;

// Function declarations
void update_force(particle_list &);
inline scalar lennard_jones(scalar);
// void limit_force(particle_list &, scalar);
// void limit_velocity(particle_list &, scalar);

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

			if (T_diag > 10000 * dt)
			{
				T_diag = 0;
				cout << p[0].r.x << "\t" << p[0].r.y << endl;
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

		if (i.r.x < box_cutoff)
		{
			d = i.r.x;
			within_reach = true;
			force_direction = 1;
		}
		else if (i.r.x > width - box_cutoff)
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

			//__builtin_prefetch (&p[i+1], 1, 1);

			scalar deltax = p[i].r.x - p[j].r.x;
			scalar deltay = p[i].r.y - p[j].r.y;

			if (abs(p[i].r.y - p[j].r.y - height) < abs(deltay))
				deltay = p[i].r.y - p[j].r.y - height;
			else if (abs(p[i].r.y - p[j].r.y + height) < abs(deltay))
				deltay = p[i].r.y - p[j].r.y + height;

			if (abs(deltax) < box_cutoff && abs(deltay) < box_cutoff)
			{
				scalar r = sqrt(deltax * deltax + deltay * deltay);
				scalar F = lennard_jones(r);

				scalar Fx = F * deltax / r;
				scalar Fy = F * deltay / r;

				p[i].F += vec(-Fx, -Fy);
				p[j].F += vec(+Fx, +Fy);
			}
		}
	}
}

void limit_force(particle_list &p, scalar max_force)
{
	for (auto &i : p)
	{
		if (abs(i.F.x) > max_force)
			i.F.x = i.F.x / abs(i.F.x) * max_force;

		if (abs(i.F.y) > max_force)
			i.F.y = i.F.y / abs(i.F.y) * max_force;
	}
}

void limit_velocity(particle_list &p, scalar max_velocity)
{
	for (auto &i : p)
	{
		if (abs(i.v.x) > max_velocity)
			i.v.x = i.v.x / abs(i.v.x) * max_velocity;

		if (abs(i.v.y) > max_velocity)
			i.v.y = i.v.y / abs(i.v.y) * max_velocity;
	}
}
inline scalar lennard_jones(scalar d)
{
	if (d < pot_size)
	{
		scalar d6 = d * d * d * d * d * d;
		return 6 * pot_size6 * (d6 - 2 * pot_size6) / (d6 * d6 * d);
	}
	else
		return 0;
}