#ifndef VIEW_H
#define VIEW_H

#include <ncurses.h>
#include <string>

#include "../api/SpotifyAPI.h"

class View {

protected:
	SpotifyAPI spotifyAPI;
	int terminalScreenWidth, terminalScreenHeight;

	WINDOW* window;

public:
	
	virtual void initialize() = 0;
	virtual void refresh() = 0;

	void prompt(WINDOW* window, unsigned int x, unsigned int y, const char* str) {
		mvwprintw(window, y, x, str);
	}
};

#endif