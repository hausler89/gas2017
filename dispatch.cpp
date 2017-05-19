#include "common.h"
#include "dispatch.h"
#include "job.h"
#include "Dispatcher.h"

using namespace std;

char coord2id(scalar x, scalar y)
{
    int box_x = int(x / box_cutoff);
    int box_y = int(y / box_cutoff);

    return box_x + box_y * num_boxes_x;
}

char id_edge(char id)
{
    if (id < num_boxes_x)
        return 'T';

    if (id >=  num_boxes_x * (num_boxes_y-1))
        return 'B';

    if (id % num_boxes_x == 0)
        return 'L';

    if (id % num_boxes_x == num_boxes_x - 1)
        return 'R';

    return '.';
}