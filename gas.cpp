#include <iostream>
#include <vector>

using namespace std;

struct vec;
struct particle;
typedef float scalar;
typedef vector<particle> particle_list;

struct vec
{
    scalar x;
    scalar y;

    vec()
    {
        x = 0;
        y = 0;
    }

    vec(scalar ix, scalar iy)
    {
        x = ix;
        y = iy;
    }
};

struct particle
{
    vec x;
    vec v;
    vec F;
    vec pF;
};

int main()
{
    const size_t N = 100;
    particle_list p(N);;


}