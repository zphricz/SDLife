#include <SDL2/SDL.h>
#include <iostream>
#include <thread>
#include "Game.h"
#include "Screen.h"

using namespace std;

static void error(char * name) {
    printf("Usage: %s [Board_x Board_y] [Screen_x Screen_y] [nthreads = 4]\n", name);
    exit(1);
}
int main(int argc, char* argv[]) {
    bool default_screen;
    bool default_cells;
    bool default_threads;
    int screen_width;
    int screen_height;
    int num_cells_x;
    int num_cells_y;
    int num_threads;
    SDL_DisplayMode display;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        exit(1);
    }
    SDL_GetCurrentDisplayMode(0, &display);
    atexit(SDL_Quit);
    srand(time(NULL));

    switch (argc) {
    case 1:
        default_screen = true;
        default_cells = true;
        default_threads = true;
        break;
    case 3:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        default_screen = true;
        default_cells = false;
        default_threads = true;
        if (num_cells_x <= 0 || num_cells_y <= 0) {
            error(argv[0]);
        }
        break;
    case 5:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = atoi(argv[3]);
        screen_height = atoi(argv[4]);
        default_screen = false;
        default_cells = false;
        default_threads = true;
        if (num_cells_x <= 0 || num_cells_y <= 0 || screen_width <= 0 || screen_height <= 0) {
            error(argv[0]);
        }
        break;
    case 6:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = atoi(argv[3]);
        screen_height = atoi(argv[4]);
        num_threads = atoi(argv[5]);
        default_screen = false;
        default_cells = false;
        default_threads = false;
        if (num_cells_x <= 0 || num_cells_y <= 0 || screen_width <= 0 || screen_height <= 0) {
            error(argv[0]);
        }
        break;
    default:
        error(argv[0]);
    }

    if (default_screen) {
        screen_width = display.w;
        screen_height = display.h;
    }
    if (default_cells) {
        num_cells_x = screen_width / 2;
        num_cells_y = screen_height / 2;
    }
    if (default_threads) {
        num_threads = thread::hardware_concurrency();
    }

    bool full_screen;
    if (screen_width == display.w && screen_height == display.h) {
        full_screen = true;
    } else {
        full_screen = false;
    }

    Screen scr(screen_width, screen_height, full_screen, "Life", true, true);
    Game g(num_cells_x, num_cells_y, &scr, num_threads);
    g.run();
    return 0;
}

