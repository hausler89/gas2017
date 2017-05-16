#include "force.h"
#include "vec.h"
#include "common.h"
#include <cmath>

using namespace std;

// Recalculate the forces acting on the particles.
// Will backup the previous force to the pV member of the particles.
void update_force(particle_list &p)
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

    // Calculating the forces for all particle combinations
    for (size_t i = 0; i < p.size() - 1; ++i)
    {
        for (size_t j = i + 1; j < p.size(); ++j)
        {
            // Displacement "vector" from p[i] to p[j]
            scalar deltax = p[i].r.x - p[j].r.x;
            scalar deltay = p[i].r.y - p[j].r.y;

            // Periodic boundaries on north and south wall
            // Check if the distance to a parallel transported "copy"
            // of the second planet is shorter. We will only calculate
            // the force to ONE SINGULAR version of the particle, assuming
            // that the potential is always smaller than the domain
            if (abs(p[i].r.y - p[j].r.y - height) < abs(deltay))
                deltay = p[i].r.y - p[j].r.y - height;
            else if (abs(p[i].r.y - p[j].r.y + height) < abs(deltay))
                deltay = p[i].r.y - p[j].r.y + height;

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
                p[i].F += vec(-Fx, -Fy);
                p[j].F += vec(+Fx, +Fy);
            }
        }
    }
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