#include "common.h"
#include <vector>
#include "job.h"

using namespace std;

struct id_vec
{
    int x;
    int y;
}

struct Dispatcher
{

    // Dispatch happens in 3x3 = 9 distinct phases
    // to avoid data races.
    int current_phase;

    // Number of jobs in a phase
    int number_of_jobs[9];

    // Number of handed out jobs in a phase
    int handed_out_jobs[9];

    // Jobs of this phase
    vector<job> jobs[9];

    Dispatcher()
    {
        current_phase = 0;

        // Create the jobs:
        // We create "agents" that are spaced 2 boxes from each
        // other. Each of the agent creates a job that is pushed
        // to the job list. Next we walk the agents to their next
        // block and repeat the process, sparing the combinations we've
        // already did before

        int agents_x = int(num_boxes_x / 3) + 1; // Non-periodic boundary
        int agents_y = int(num_boxes_y / 3);     // Periodic boundary

        // Total number of agents
        int num_agents = agents_x * agents_y;

        // List of agents, containin the id he's pointing to
        vector<id_vec> agent(num_agents);

        // Check if the box coordinates are valid and withing
        // the simulation domain
        bool valid_id(id_vec v)
        {
            if(v.x < 0 || v.x >= num_boxes_x || v.y < 0 || v.y > num_boxes_y)
                return false;

            return true;
        }

        int vec2id(id_vec v)
        {
            if(v.x >= num_boxes_x)
                return -1;
            if(v.y >= num_boxes_y)
                return -1;
            
        }

        // Initialize the agents
        for (int i = 0; i < agents_x; ++i)
            for (int j = 0; j < agents_y; ++j)
            {
                int idx = i + j * agents_x;
                agent[idx] = 3 * i + 3 * j * num_boxes_x;
            }
    }
};