#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include <curses.h>
#include <unistd.h>
#include "common.h"
#include "vec.h"
#include "particle.h"
#include "gui.h"
#include "dispatch.h"

using namespace std;

void init_gui()
{

	// Init Ncurses screen
	initscr();	 // Create curses window
	start_color(); // Use color output
	curs_set(0);   // Hide cursor

	// Define some color schemes
	init_pair(1, COLOR_WHITE, COLOR_BLACK); // Text
	init_pair(2, COLOR_WHITE, COLOR_RED);   // Planets
	init_pair(3, COLOR_GREEN, COLOR_BLACK); // Trails

	// Set output to bold to get foreground colors
	attron(A_BOLD);
}

void draw_particles(const particle_list &p)
{
	// Get screen size
	int screen_x, screen_y;
	getmaxyx(stdscr, screen_y, screen_x);
	//clear();
	for (auto i : p)
	{

		double x_rel = i.r.x / width; // Relative position according to fov
		double y_rel = i.r.y / height;

		int pos_x = x_rel * screen_x;
		int pos_y = y_rel * screen_y;

		char id;
		id = '0' + coord2id(i.r.x, i.r.y);
		id = id_edge(coord2id(i.r.x, i.r.y));

		mvaddch(pos_y, pos_x, id);
	}
}