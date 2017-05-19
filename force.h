#pragma once

#include "particle.h"

void update_force(particle_list &p, const vector<char> box);
inline scalar lennard_jones(scalar d);