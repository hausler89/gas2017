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