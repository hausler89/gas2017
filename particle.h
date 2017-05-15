#pragma once
#include <vector>
#include "vec.h"
#include <iostream>

struct particle;

using namespace std;

typedef vector<particle> particle_list;

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