// Draw all planets to the screen
// FOV is the radius of the guaranteed visible area
#include "gui.h"
#include <algorithm>
#include <curses.h>
#include "global_constants.h"
#include <vector>

// For saving the trails
struct pixel
{
    int x;
    int y;
    int age;
};

void draw_planets(Planet *planet, int planet_count, double fov)
{
    // Static list of the trails of the planets
    static std::vector<pixel> trail;

    // This is the highest index with aged trail points. All elements
    // of trail prior to this index will be deleted
    int oldest_index = -1;

    // Delete screen
    clear();

    // Print the trail
    for (int i = 0; i < trail.size(); ++i)
    {
        if (trail[i].age < 100)
        {
            attron(COLOR_PAIR(3));
            //attron(A_BOLD);

            mvaddch(trail[i].y, trail[i].x, '.');
            trail[i].age++;
        }
        else
            // Ages will always be ascending
            oldest_index = i;
    }

    // Remove the old trail points
    if (oldest_index > 0 && oldest_index < trail.size())
        trail.erase(trail.begin(), trail.begin() + oldest_index - 1);

    // Get screen size
    int screen_x, screen_y;
    getmaxyx(stdscr, screen_y, screen_x);

    int screen_min = std::min(screen_x, screen_y);

    // Iterate over all planets
    for (int i = planet_count - 1; i >= 0; --i)
    {
        // Calculate the position on the screen
        double x_rel = planet[i].x / fov; // Relative position according to fov
        double y_rel = planet[i].y / fov;

        double aspect_ratio = 18. / 8.;

        int pos_x = screen_x / 2 + aspect_ratio * screen_min / 2 * x_rel + 0.5;
        int pos_y = screen_y / 2 + screen_min / 2 * y_rel + 0.5;

        if (pos_x > 0 && pos_y > 0 && pos_x < screen_x && pos_y < screen_y)
        {
            pixel new_px = {pos_x, pos_y, 0};
            trail.push_back(new_px);
            attron(COLOR_PAIR(2));
            //attron(A_BOLD);
            attron(A_STANDOUT);
            mvaddch(pos_y, pos_x, planet[i].symbol);
            attroff(A_STANDOUT);
            //mvprintw(7, 0, "Trailpoints: %d", trail.size());
        }
    }
    //refresh();
}