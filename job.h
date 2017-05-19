#pragma once
#include <vector>

using namespace std;

struct job
{
    // Boxes that need to be calculated
    vector<char> id;

    // Insert a box to this job
    void add_id(char new_id)
    {
        id.push_back(new_id);
    }
};