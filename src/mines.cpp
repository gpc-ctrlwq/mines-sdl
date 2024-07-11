
/** Author: Gene P. Cross
 * 	Description: main file for mines clone
 **/

#include <SDL/SDL.h>
#include "mtrand.h" 
#include <stdlib.h>
#include <time.h>

#define WIDTH 600
#define HEIGHT 600
#define TILE_WIDTH 20
#define TILE_HEIGHT 20
#define MAP_WIDTH (WIDTH / TILE_WIDTH)
#define MAP_HEIGHT (HEIGHT / TILE_HEIGHT)

// globals //
struct Tile {
	int imgIndex;
	bool isMine;
	int mineCount;
};
Tile map[MAP_HEIGHT][MAP_WIDTH];
MTRand_int32 mtRand(time(NULL));
SDL_Surface* screen;
SDL_Surface* images[12];
/////////////

// Forward declarations //
void drawScreen();
void leftClick(SDL_Event event);
void rightClick(SDL_Event event);
void floodReveal(int x, int y);

// entry point //
int main() {
	printf("Starting\n");
	SDL_Event event;
	int difficulty = 7; // higher is easier

	// load images //
	printf("Loading..");
	images[0] = SDL_LoadBMP("./data/empty.bmp");
	images[1] = SDL_LoadBMP("./data/1.bmp");
	images[2] = SDL_LoadBMP("./data/2.bmp");
	images[3] = SDL_LoadBMP("./data/3.bmp");
	images[4] = SDL_LoadBMP("./data/4.bmp");
	images[5] = SDL_LoadBMP("./data/5.bmp");
	images[6] = SDL_LoadBMP("./data/6.bmp");
	images[7] = SDL_LoadBMP("./data/7.bmp");
	images[8] = SDL_LoadBMP("./data/8.bmp");
	images[9] = SDL_LoadBMP("./data/tile.bmp");
	images[10] = SDL_LoadBMP("./data/flag.bmp");
	images[11] = SDL_LoadBMP("./data/mine.bmp");
	printf("DONE\n");


	// init SDL //
	printf("Init SDL..");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24,
				 SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE);
		if (screen == NULL) {
			printf("Could not init SDL video mode: %s\n)", SDL_GetError());
			return 1;
		}
		SDL_WM_SetCaption(">>) MINES (<<", "MINES");
	}
	else {
		printf("Could not init SDL: %s\n", SDL_GetError());
		return 1;
	}
	printf("DONE\n");


	// init map // 
	printf("Init map..");
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			map[y][x].imgIndex = 9;
			if (mtRand() % difficulty == 0) {
				map[y][x].isMine = true;
			}
		}
	}
	printf("DONE\n");

	
	// get mineCount for each tile //
	printf("Get mineCounts..");
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			// if mine continue, no need to count //
			if (map[y][x].isMine) {
				continue;
			}
			// search surrounding tiles //
			for (int search_y = -1; search_y <= 1; search_y++) {
				for (int search_x = -1; search_x <= 1; search_x++) {
					// bounds check //
					if (y + search_y < 0 || x + search_x < 0) {
						continue;
					}
					if (y + search_y > MAP_HEIGHT || x + search_x > MAP_WIDTH) {
						continue;
					}
					// if not current tile //
					if (!(search_y == 0 && search_x == 0)) {
						// if mine add to count //
						if ( map[y + search_y][x + search_x].isMine ) {
							map[y][x].mineCount++;
						}
					}
				}
			}
		}
	}	
	printf("DONE\n");
	

	printf("First drawScreen()..");
	drawScreen();
	SDL_Flip(screen);
	printf("DONE\n");


	// main loop //
	printf("Entering main loop..\n");
	while (true) {
		
		// poll for events //
		while (SDL_PollEvent(&event)) {

			switch (event.type) {
				// quit event //
				case SDL_QUIT:
					printf("Quiting\n");
					return 0;

				// mouse event //
				case SDL_MOUSEBUTTONDOWN:
					printf("mouse button down: ");
					if (event.button.button == SDL_BUTTON_LEFT) {
						leftClick(event);
						printf("LEFT\n");
					}
					else if (event.button.button == SDL_BUTTON_RIGHT) {
						rightClick(event);
						printf("RIGHT\n");
					}
					break;

				// default //
				default:
					break;
			}
		}

	} // end of while (main loop) //	

	printf("Exiting\n");
	return 0;
} // end of main() //



void drawScreen() {
	SDL_Rect dest;

	// loop through map and blit tiles to screen //
	for (int y = 0; y < (HEIGHT / TILE_HEIGHT); y++) {
		for (int x = 0; x < (WIDTH / TILE_WIDTH); x++) {
			dest.y = y * TILE_HEIGHT;
			dest.x = x * TILE_WIDTH;

			SDL_BlitSurface(images[ map[y][x].imgIndex ], NULL, screen, &dest);
		}
	}
}



void leftClick(SDL_Event event) {
	// get coords of mouse click //
	int xpos = event.button.x / 20;
	int ypos = event.button.y / 20;

	// if mine, game over, show all mines //
	if (map[ypos][xpos].isMine) {
		printf("game over!\n");
	
		// show all mines
		for (int y = 0; y < MAP_HEIGHT; y++) {
			for (int x = 0; x < MAP_WIDTH; x++) {
				if (map[y][x].isMine) {
					map[y][x].imgIndex = 11;
				}
			}
		}
	}
	// otherwise, show number of surrounding mines //
	else {
		floodReveal(ypos, xpos);
	}

	// draw and update screen //
	drawScreen();
	SDL_Flip(screen);
}


void rightClick(SDL_Event event) {
	// get coords of mouse click //
	int xpos = event.button.x / 20;
	int ypos = event.button.y / 20;

	// place/remove flag //
	map[ypos][xpos].imgIndex = 10;
	
	// draw and update screen //
	drawScreen();
	SDL_Flip(screen);
}


void floodReveal(int y, int x) {
	if (map[y][x].imgIndex == 9) {
		map[y][x].imgIndex = map[y][x].mineCount;

		if (map[y][x].mineCount == 0) {
			printf("here\n");
			// check adjacent tiles //
			for (int search_y = -1; search_y <= 1; search_y++) {
				for (int search_x = -1; search_x <= 1; search_x++) {
					if (!(search_y == 0 && search_x == 0)) {
						// bounds check //
						if (y + search_y < 0 || x + search_x < 0) {
							continue;
						}
						if (y + search_y > MAP_HEIGHT || x + search_x > MAP_WIDTH) {
							continue;
						}
						floodReveal(y + search_y, x + search_x);
					}
				}
			}
		}
	}
}
