#include <iostream>
#include "quickcg.h"
#include "Game.h"
#include "Screen.h"

using namespace QuickCG;
using namespace std;

const double FPS = 60.0;

static ColorRGB color(255, 0, 0); // Start as red
static const ColorRGB black(0, 0, 0);
static const ColorRGB white(255, 255, 255);

static enum {UP_GREEN,   DOWN_RED, UP_BLUE,
             DOWN_GREEN, UP_RED,   DOWN_BLUE} color_state = UP_GREEN;

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
        }
        else
            color.g += rate;
        break;
    case DOWN_RED:
        if ((int)color.r - rate <= 0) {
            color.r = 0;
            color_state = UP_BLUE;
        }
        else
            color.r -= rate;
        break;
    case UP_BLUE:
        if ((int)color.b + rate >= 255) {
            color.b = 255;
            color_state = DOWN_GREEN;
        }
        else
            color.b += rate;
        break;
    case DOWN_GREEN:
        if ((int)color.g - rate <= 0) {
            color.g = 0;
            color_state = UP_RED;
        }
        else
            color.g -= rate;
        break;
    case UP_RED:
        if ((int)color.r + rate >= 255) {
            color.r = 255;
            color_state = DOWN_BLUE;
        }
        else
            color.r += rate;
        break;
    case DOWN_BLUE:
        if ((int)color.b - rate <= 0) {
            color.b = 0;
            color_state = UP_GREEN;
        }
        else
            color.b -= rate;
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[]) {
    bool default_screen;
    bool default_cells;
    bool show_fps = false;
    int screen_width;
    int screen_height;
    int num_cells_x;
    int num_cells_y;
    int last_clicked_cell_x = -1;
    int last_clicked_cell_y = -1;
    int rand_percent = 50;
    bool step = false;
    bool simulate = false;
    bool do_color = true;
    unsigned int old_time;
    unsigned int time;
    unsigned int sleep_to;
    unsigned int ms_diff = (unsigned int)(1000.0 / FPS);
    unsigned int end_print_time = 500;
    string message = "DAY AND NIGHT";
    SDL_Event event = {0};

    srand(getTime());

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

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    atexit(SDL_Quit);

    const SDL_VideoInfo * info = SDL_GetVideoInfo();
    if (default_screen) {
        screen_width = info->current_w;
        screen_height = info->current_h;
    }
    if (default_cells) {
        num_cells_x = info->current_w / 2;
        num_cells_y = info->current_h / 2;
    }

    bool full_screen;
    if (screen_width == info->current_w && screen_height == info->current_h) {
        full_screen = true;
    } else {
        full_screen = false;
    }

    Screen scr(screen_width, screen_height, full_screen, "Life");
    Game g(num_cells_x, num_cells_y, &scr);
    g.init_cells(rand_percent);
    time = getTicks();
    sleep_to = time + ms_diff;

    // Main loop
    while (true) {
        scr.cls();

        if (do_color) {
            scr.setColor(color);
        } else {
            scr.setColor(white);
        }

        if (simulate || step) {
            g.iterate();
            change_color(2);
        } else {
            g.draw_cells();
        }

        SDL_PollEvent(&event);

        // Handle mouse clicks
        int mouse_x, mouse_y;
        bool lmb = false, rmb = false;
        getMouseState(mouse_x, mouse_y, lmb, rmb);
        if (lmb || rmb) {
            int cell_y = mouse_y * num_cells_y / screen_height;
            int cell_x = mouse_x * num_cells_x / screen_width;
            if (!(last_clicked_cell_x == cell_x && last_clicked_cell_y == cell_y)) {
                if (last_clicked_cell_x == -1) {
                    last_clicked_cell_x = cell_x;
                    last_clicked_cell_y = cell_y;
                }
                int dx = cell_x - last_clicked_cell_x;
                int dy = cell_y - last_clicked_cell_y;

                if (abs(dx) > abs(dy)) {
                    int increment = dx < 0 ? -1 : 1;
                    for(int x = last_clicked_cell_x; x != cell_x; x += increment) {
                        int y = last_clicked_cell_y + dy * (x - last_clicked_cell_x) / dx;
                        if (lmb) {
                            g.cell_at(x, y) = 1;
                        } else if (rmb) {
                            g.cell_at(x, y) = 0;
                        }
                    }
                }
                else {
                    int increment = dy < 0 ? -1 : 1;
                    for(int y = last_clicked_cell_y; y != cell_y; y += increment) {
                        int x = last_clicked_cell_x + dx * (y - last_clicked_cell_y) / dy;
                        if (lmb) {
                            g.cell_at(x, y) = 1;
                        } else if (rmb) {
                            g.cell_at(x, y) = 0;
                        }
                    }
                }
                if (lmb) {
                    g.cell_at(cell_x, cell_y) = 1;
                } else if (rmb) {
                    g.cell_at(cell_x, cell_y) = 0;
                }
            }
            last_clicked_cell_x = cell_x;
            last_clicked_cell_y = cell_y;
        }
        else {
            last_clicked_cell_x = -1;
            last_clicked_cell_y = -1;
        }

        // Handle key presses
        readKeys();
        {
            static bool pressed = false;
            if (keyDown(SDLK_1)) {
                if (!pressed) {
                    message = g.switch_game();
                    pressed = true;
                    end_print_time = getTicks() + 500;
                }
            } else {
                pressed = false;
            }
        }
        {
            static bool pressed = false;
            if (keyDown(SDLK_2)) {
                if (!pressed) {
                    message = g.switch_boundary();
                    pressed = true;
                    end_print_time = getTicks() + 500;
                }
            } else {
                pressed = false;
            }
        }
        {
            static bool pressed = false;
            if (keyDown(SDLK_3)) {
                if (!pressed) {
                    pressed = true;
                    do_color = !do_color;
                }
            } else {
                pressed = false;
            }
        }
        if (keyDown(SDLK_4)) {
            change_color(10);
        }
        {
            static bool pressed = false;
            if (keyDown(SDLK_5)) {
                if (!pressed) {
                    pressed = true;
                    show_fps ^= 1;
                }
            } else {
                pressed = false;
            }
        }
        {
            static bool pressed = false;
            if (keyDown(SDLK_RETURN)) {
                if (!pressed) {
                    pressed = true;
                    simulate = !simulate;
                    if (simulate)
                        message = "START";
                    else
                        message = "STOP";
                    end_print_time = getTicks() + 500;
                }
            } else {
                pressed = false;
            }
        }
        if (keyDown(SDLK_LEFT)) {
            rand_percent = 0;
            g.init_cells(rand_percent);
            end_print_time = getTicks() + 500;
            ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_RIGHT)) {
            rand_percent = 100;
            g.init_cells(rand_percent);
            end_print_time = getTicks() + 500;
            ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_PAGEDOWN) || keyDown(SDLK_DOWN)) {
            rand_percent = rand_percent == 0 ? 0 : rand_percent - 1;
            g.init_cells(rand_percent);
            end_print_time = getTicks() + 500;
            ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_PAGEUP) || keyDown(SDLK_UP)) {
            rand_percent = rand_percent == 100 ? 100 : rand_percent + 1;
            g.init_cells(rand_percent);
            end_print_time = getTicks() + 500;
            ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        {
            static bool pressed = false;
            if (keyDown(SDLK_SPACE)) {
                if (!pressed) {
                    pressed = true;
                    step = true;
                } else {
                    step = false;
                }
            } else {
                pressed = false;
                step = false;
            }
        }
        if (keyDown(SDLK_ESCAPE)) {
            break;
        }
    
        old_time = time;
        time = getTicks();
        if (show_fps) {
            scr.print(1000.0 / (time - old_time));
        } else {
            // Display messages
            if (time < end_print_time) {
                scr.print(message);
            }
        }
        scr.redraw(); // Draw SDL pixel buffer

        // Sleep so that FPS is maintained
        if (sleep_to > time) {
            SDL_Delay(sleep_to - time);
            sleep_to += ms_diff;
        }
        else {
            sleep_to += ((time - sleep_to)/ms_diff + 1) * ms_diff;
        }
    }
    return 0;
}

