#include "common.h"
#include "dispatch.h"

using namespace std;

char coord2id(scalar x, scalar y)
{
    int box_x = int(x / box_cutoff);
    int box_y = int(y / box_cutoff);

    return box_x + box_y * (num_boxes_x + 1);
}

char id_edge(char id)
{
    if (id % (num_boxes_x + 1) == 0)
        return 1;

    if (id % (num_boxes_x + 1) == num_boxes_x)
        return 2;

    return 0;
}

// char id_neighbor(const char id1, char id2)
// {

//     if (id1 >= (num_boxes_x + 1) * num_boxes_y && id2 < num_boxes_x + 1)
//         id2 -= (num_boxes_x + 1) * num_boxes_y;

//     // if (id1 < num_boxes_x + 1)
//     //     return 'y';

//     if (!id_edge(id1))
//         if (abs(id1 - id2) == 1 ||
//             abs(id1 - id2 + num_boxes_x + 1) < 2 ||
//             abs(id1 - id2 - num_boxes_x - 1) < 2)
//             return 'O';

//     if (id_edge(id1) == 1)
//         if (id1 - id2 == -1 ||
//             id1 - id2 + num_boxes_x + 1 == -1 ||
//             id1 - id2 - num_boxes_x - 1 == -1 ||
//             id1 - id2 + num_boxes_x + 1 == 0 ||
//             id1 - id2 - num_boxes_x - 1 == 0)
//             return 'O';

//     if (id_edge(id1) == 2)
//         if (id1 - id2 == 1 ||
//             id1 - id2 + num_boxes_x + 1 == 1 ||
//             id1 - id2 - num_boxes_x - 1 == 1 ||
//             id1 - id2 + num_boxes_x + 1 == 0 ||
//             id1 - id2 - num_boxes_x - 1 == 0)
//             return 'O';

//     return '.';
// }