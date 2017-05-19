#include "common.h"
#include <vector>
#include "job.h"

using namespace std;

struct id_vec
{
	int x;
	int y;

	id_vec()
	{
		x = 0;
		y = 0;
	}

	id_vec(int ix, int iy)
	{
		x = ix;
		y = iy;
	}
};

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

	// Convert box coordinates to box id
	int vec2id(id_vec v)
	{
		if (valid_id(v))
		{
			return v.x + v.y * num_boxes_x;
		}
		else
			return -1;
	}

	bool valid_id(id_vec v)
	{
		if (v.x < 0 || v.x >= num_boxes_x || v.y < 0 || v.y > num_boxes_y)
			return false;

		return true;
	}

	// Move an id_vec around, considering the periodic boundaries
	void walk_up(id_vec &v)
	{
		v.y += 1;
		if (v.y >= num_boxes_y)
			v.y -= num_boxes_y;
	}

	void walk_down(id_vec &v)
	{
		v.y -= 1;
		if (v.y < 0)
			v.y += num_boxes_y;
	}

	void walk_left(id_vec &v)
	{
		v.x -= 1;
	}

	void walk_right(id_vec &v)
	{
		v.x += 1;
	}

	Dispatcher()
	{

		// Create the jobs:
		// We create "agents" that are spaced 2 boxes from each
		// other. Each of the agent creates a job that is pushed
		// to the job list. Next we walk the agents to their next
		// block and repeat the process, sparing the combinations we've
		// already did before

		int agents_x = int(num_boxes_x / 3) + 1; // Non-periodic boundary
		int agents_y = int(num_boxes_y / 3);	 // Periodic boundary

		// Total number of agents
		int num_agents = agents_x * agents_y;

		// List of agents, containin the id he's pointing to
		vector<id_vec> agent(num_agents);

		// Check if the box coordinates are valid and withing
		// the simulation domain

		// Initialize the agents
		for (int i = 0; i < agents_x; ++i)
			for (int j = 0; j < agents_y; ++j)
			{
				int idx = i + j * agents_x;
				agent[idx] = id_vec(3 * i, 3 * j);
			}

		// Now we create the phases.

		// 1st phase, all of the surrounding ids will be calculated,
		// as well as the cell itself

		current_phase = 0;

		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// N
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// NE
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// E
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// W
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// NW
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_down(A);
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 2nd phase, we spare W now, since we already did this combo before.
		current_phase = 1;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
			walk_right(agent[k]);

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// N
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// NE
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// E
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// W (SPARED)
				walk_up(A);

				// NW
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_down(A);
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 3rd phase, we spare W and E, since we already did these combos before.
		current_phase = 2;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
			walk_right(agent[k]);

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// N
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// NE
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// E (SPARED)
				walk_down(A);

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// W (SPARED)
				walk_up(A);

				// NW
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_down(A);
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 4th phase, sparing NW, N and NE.
		current_phase = 3;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
		{
			walk_left(agent[k]);
			walk_left(agent[k]);
			walk_down(agent[k]);
		}
		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// E
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// W
				walk_up(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 5th phase, Sparing NW, N, NE and W.
		current_phase = 4;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
			walk_right(agent[k]);

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// E
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_up(A);
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 6th phase, NW, N, NE, E and W, since we already did these combos before.
		current_phase = 5;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
			walk_right(agent[k]);

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// E (SPARED)
				walk_right(A);

				// SE
				walk_down(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// S
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// SW
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// Back to origin
				walk_up(A);
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 7th phase, only E and W.
		current_phase = 6;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
		{
			walk_left(agent[k]);
			walk_left(agent[k]);
			walk_down(agent[k]);
		}

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// E
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));

				// E
				walk_left(A);
				walk_left(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));


				// Back to origin
				walk_right(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}


		// 8th phase, only E.
		current_phase = 7;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
		{
			walk_right(agent[k]);
		}

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);

				// Now we walk around the origin and add the box to the
				// job if its valid.

				// E
				walk_right(A);
				if (valid_id(A))
					new_job.add_id(vec2id(A));


				// Back to origin
				walk_left(A);

				// Add the job to this phase's list
				jobs[current_phase].push_back(new_job);
			}
		}

		// 9th phase, (only self).
		current_phase = 8;

		// First, move all Agents
		for (int k = 0; k < num_agents; ++k)
		{
			walk_right(agent[k]);
		}

		// And create the new jobs
		for (int k = 0; k < num_agents; ++k)
		{
			id_vec &A = agent[k];
			// Check if the agent is inside the domain, otherwise we will
			// ignore him for now. He might walk back into the domain later
			if (valid_id(A))
			{

				// Create a new job that will be initialized and then
				// pushed to the job list
				job new_job;
				new_job.origin = vec2id(A);
			}
		}
	}
};