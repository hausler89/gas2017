#pragma once

#include "particle.h"
#include "job.h"

void update_force(particle_list &p, const vector<vector<int>> box);
inline scalar lennard_jones(scalar d);
int next_origin(int i0, const vector<int> &box, job J);
int next_particle(int i0, const vector<int> &box, job J);