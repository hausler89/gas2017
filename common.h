#pragma once

#include <cstdlib>

// Scalar is the floating point datatype for the sim 
typedef double scalar;

// Global variables (initialized in gas.cpp)
extern const size_t N;
extern const scalar box_cutoff;
extern const scalar pot_size;
extern const scalar pot_size6;
extern const scalar height;
extern const scalar width;

extern const int num_boxes_x;
extern const int num_boxes_y;

extern const int grid_h;
extern const int grid_w;

extern const scalar velocity_max;
extern const scalar dt;
extern const int num_boxes;