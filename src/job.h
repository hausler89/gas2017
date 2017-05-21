#pragma once
#include <vector>

using namespace std;

struct job
{
    // Center cell that is the origin of all calculations
    int origin;

    // Boxes that interact with the origin.
    vector<int> id;

    // Insert a box to this job
    void add_id(int new_id)
    {
        id.push_back(new_id);
    }
};