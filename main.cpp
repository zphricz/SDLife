#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Game.h"
#include "Screen.h"

using namespace std;

const Uint32 frames_per_fps_show = 5;
static Color color{255, 0, 0}; // Start as red
static enum {UP_GREEN,   DOWN_RED, UP_BLUE,
             DOWN_GREEN, UP_RED,   DOWN_BLUE
            } color_state = UP_GREEN;

static void error(char * name) {
    printf("Usage: %s [Board_x Board_y] [Screen_x Screen_y]\n", name);
    exit(1);
}

static void change_color(int rate) {
    switch (color_state) {
    case UP_GREEN:
        if ((int)color.g + rate >= 255) {
            color.g = 255;
            color_state = DOWN_RED;
        } else
            color.g += rate;
        break;
    case DOWN_RED:
        if ((int)color.r - rate <= 0) {
            color.r = 0;
            color_state = UP_BLUE;
        } else
            color.r -= rate;
        break;
    case UP_BLUE:
        if ((int)color.b + rate >= 255) {
            color.b = 255;
            color_state = DOWN_GREEN;
        } else
            color.b += rate;
        break;
    case DOWN_GREEN:
        if ((int)color.g - rate <= 0) {
            color.g = 0;
            color_state = UP_RED;
        } else
            color.g -= rate;
        break;
    case UP_RED:
        if ((int)color.r + rate >= 255) {
            color.r = 255;
            color_state = DOWN_BLUE;
        } else
            color.r += rate;
        break;
    case DOWN_BLUE:
        if ((int)color.b - rate <= 0) {
            color.b = 0;
            color_state = UP_GREEN;
        } else
            color.b -= rate;
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    bool default_screen;
    bool default_cells;
    bool show_fps = true;
    int screen_width;
    int screen_height;
    int num_cells_x;
    int num_cells_y;
    int rand_percent = 50;
    bool step = false;
    bool simulate = false;
    bool do_color = true;
    Uint32 current_time;
    Uint32 fps_start_time;
    int fps_counter = 0;
    SDL_DisplayMode display;
    SDL_Event event;

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
        break;
    case 3:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        default_screen = true;
        default_cells = false;
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

    bool full_screen;
    if (screen_width == display.w && screen_height == display.h) {
        full_screen = true;
    } else {
        full_screen = false;
    }

    Screen scr(screen_width, screen_height, full_screen, "Life");
    Game g(num_cells_x, num_cells_y, &scr);
    g.init_cells(rand_percent);
    cout << "GAME: DAY AND NIGHT" << endl;
    fps_start_time = SDL_GetTicks();

    // Main loop
    while (true) {
        scr.cls();
        if (do_color) {
            scr.set_color(color);
        } else {
            scr.set_color(255, 255, 255);
        }
        if (simulate || step) {
            g.iterate();
            change_color(2);
            step = false;
        } else {
            g.draw_cells();
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE: {
                    goto done;
                    break;
                }
                case SDLK_RETURN: {
                    simulate = !simulate;
                    break;
                }
                case SDLK_SPACE: {
                    step = true;
                    break;
                }
                case SDLK_1: {
                    cout << "GAME: " << g.switch_game() << endl;
                    break;
                }
                case SDLK_2: {
                    cout << "BOUNDARY: " << g.switch_boundary() << endl;
                    break;
                }
                case SDLK_3: {
                    do_color = !do_color;
                    break;
                }
                case SDLK_4: {
                    change_color(10);
                    break;
                }
                case SDLK_5: {
                    show_fps = !show_fps;
                    break;
                }
                case SDLK_LEFT: {
                    rand_percent = 0;
                    g.init_cells(rand_percent);
                    ostringstream convert;
                    convert << rand_percent;
                    cout << "RAND_PERCENT: " << convert.str() << "%" << endl;
                    break;
                }
                case SDLK_RIGHT: {
                    rand_percent = 100;
                    g.init_cells(rand_percent);
                    ostringstream convert;
                    convert << rand_percent;
                    cout << "RAND_PERCENT: " << convert.str() << "%" << endl;
                    break;
                }
                case SDLK_UP: {
                    if (rand_percent < 100) {
                        rand_percent++;
                    }
                    g.init_cells(rand_percent);
                    ostringstream convert;
                    convert << rand_percent;
                    cout << "RAND_PERCENT: " << convert.str() << "%" << endl;
                    break;
                }
                case SDLK_DOWN: {
                    if (rand_percent > 0) {
                        rand_percent--;
                    }
                    g.init_cells(rand_percent);
                    ostringstream convert;
                    convert << rand_percent;
                    cout << "RAND_PERCENT: " << convert.str() << "%" << endl;
                    break;
                }
                default: {
                    break;
                }
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                int x = num_cells_x * event.button.x / screen_width;
                int y = num_cells_y * event.button.y / screen_height;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    g.cell_at(x, y) = 1;
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    g.cell_at(x, y) = 0;
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                int end_x = event.motion.x;
                int end_y = event.motion.y;
                int start_x = end_x - event.motion.xrel;
                int start_y = end_y - event.motion.yrel;
                int start_cell_x = start_x * num_cells_x / screen_width;
                int start_cell_y = start_y * num_cells_y / screen_height;
                int end_cell_x = end_x * num_cells_x / screen_width;
                int end_cell_y = end_y * num_cells_y / screen_height;
                int dx = end_cell_x - start_cell_x;
                int dy = end_cell_y - start_cell_y;
                const bool left_click = event.motion.state & SDL_BUTTON_LMASK;
                const bool right_click = event.motion.state & SDL_BUTTON_RMASK;
                if (!left_click && !right_click) {
                    break;
                }
                if (abs(dx) > abs(dy)) {
                    int increment = dx < 0 ? -1 : 1;
                    for(int x = start_cell_x; x != end_cell_x; x += increment) {
                        int y = start_cell_y + dy * (x - start_cell_x) / dx;
                        if (left_click) {
                            g.cell_at(x, y) = 1;
                        } else {
                            g.cell_at(x, y) = 0;
                        }
                    }
                } else {
                    int increment = dy < 0 ? -1 : 1;
                    for(int y = start_cell_y; y != end_cell_y; y += increment) {
                        int x = start_cell_x + dx * (y - start_cell_y) / dy;
                        if (left_click) {
                            g.cell_at(x, y) = 1;
                        } else {
                            g.cell_at(x, y) = 0;
                        }
                    }
                }
                break;
            }
            default: {
                 break;
            }
            }
        }

        fps_counter++;
        if (fps_counter == frames_per_fps_show) {
            current_time = SDL_GetTicks();
            ostringstream strout;
            strout << fixed << setprecision(2) << 1000.0 * frames_per_fps_show / (current_time - fps_start_time);
            if (show_fps) {
                cout << "FPS: " <<strout.str() << endl;
            }
            fps_counter = 0;
            fps_start_time = current_time;
        }

        scr.commit_screen(); // Draw SDL pixel buffer
    }
done:
    return 0;
}

