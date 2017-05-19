#pragma once

#include "particle.h"
#include "job.h"

void update_force(particle_list &p, const vector<char> box);
inline scalar lennard_jones(scalar d);
int next_origin(int i0, const vector<char> &box, job J);
int next_particle(int i0, const vector<char> &box, job J);