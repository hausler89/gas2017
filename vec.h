#pragma once
#include "common.h"

using namespace std;

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

vec operator*(const scalar &lhs, const vec &rhs);