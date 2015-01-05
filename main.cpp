#include "quickcg.h"
#include <iostream>
using namespace QuickCG;
using std::cout;
using std::endl;

const double FPS = 60.0;

static bool * buffer_1;
static bool * buffer_2;
static bool * current_state;
static bool * next_state;
static int buff_width;
static int buff_height;

static ColorRGB color(255, 0, 0);
static const ColorRGB black(0, 0, 0);
static const ColorRGB white(255, 255, 255);

static enum {UP_GREEN,   DOWN_RED, UP_BLUE,
             DOWN_GREEN, UP_RED,   DOWN_BLUE} color_state = UP_GREEN;
static enum {CONWAY, SEEDS, DIAMONDS, BLOTCHES, DAY_AND_NIGHT} game = DAY_AND_NIGHT;
static enum {PACMAN, DEAD, ALIVE} boundary = PACMAN;

static bool get_current(int x, int y)
{
    return current_state[y * buff_width + x];
}

static void set_current(int x, int y, bool next)
{
    current_state[y * buff_width + x] = next;
}

static void set_next(int x, int y, bool next)
{
    next_state[y * buff_width + x] = next;
}

static void change_color(int rate)
{
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

static void init_cells(int num_cells_x, int num_cells_y, int percent)
{
    for(int y = 0; y < num_cells_y; ++y) { 
        for(int x = 0; x < num_cells_x; ++x) {
            if (percent > rand() % 100) set_current(x, y, 1);
            else set_current(x, y, 0);
        }
    }
}

static void set_boundaries(int num_cells_x, int num_cells_y)
{
    switch(boundary) {
    case PACMAN:
        set_current(-1, -1, get_current(num_cells_x - 1, num_cells_y - 1));
        set_current(num_cells_x, -1, get_current(0, num_cells_y - 1));
        set_current(-1, num_cells_y, get_current(num_cells_x - 1, 0));
        set_current(num_cells_x, num_cells_y, get_current(0, 0));
        for (int x = 0; x < num_cells_x; ++x) {
            set_current(x, -1, get_current(x, num_cells_y - 1));
            set_current(x, num_cells_y, get_current(x, 0));
        }
        for (int y = 0; y < num_cells_y; ++y) {
            set_current(-1, y, get_current(num_cells_x - 1, y));
            set_current(num_cells_x, y, get_current(0, y));
        }
        break;
    case ALIVE:
        set_current(-1, -1, 1);
        set_current(num_cells_x, -1, 1);
        set_current(-1, num_cells_y, 1);
        set_current(num_cells_x, num_cells_y, 1);
        for (int x = 0; x < num_cells_x; ++x) {
            set_current(x, -1, 1);
            set_current(x, num_cells_y, 1);
        }
        for (int y = 0; y < num_cells_y; ++y) {
            set_current(-1, y, 1);
            set_current(num_cells_x, y, 1);
        }
        break;
    case DEAD:
        set_current(-1, -1, 0);
        set_current(num_cells_x, -1, 0);
        set_current(-1, num_cells_y, 0);
        set_current(num_cells_x, num_cells_y, 0);
        for (int x = 0; x < num_cells_x; ++x) {
            set_current(x, -1, 0);
            set_current(x, num_cells_y, 0);
        }
        for (int y = 0; y < num_cells_y; ++y) {
            set_current(-1, y, 0);
            set_current(num_cells_x, y, 0);
        }
        break;
    default:
        break;
    }
}

int main(int argc, char* argv[])
{
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
    std::string message = "DAY AND NIGHT";
    SDL_Event event = {0};

    srand(getTime());

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        SDL_Quit();
        std::exit(1);
    }
    std::atexit(SDL_Quit);
    const SDL_VideoInfo * info = SDL_GetVideoInfo();
    
    switch (argc) {
    case 1:
        num_cells_x = info->current_w / 2;
        num_cells_y = info->current_h / 2;
        screen_width = info->current_w;
        screen_height = info->current_h;
        break;
    case 3:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = info->current_w;
        screen_height = info->current_h;
        break;
    case 5:
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = atoi(argv[3]);
        screen_height = atoi(argv[4]);
        break;
    default:
        printf("Usage: ./life [Board_x Board_y] [Screen_x Screen_y]\n");
        exit(1);
    }
    if (num_cells_x <= 0 || num_cells_y <= 0 || screen_width <= 0 || screen_height <= 0) {
        printf("Usage: ./life [Board_x Board_y] [Screen_x Screen_y]\n");
        exit(1);
    }

    if (screen_width == info->current_w && screen_height == info->current_h)
        screen(screen_width, screen_height, true, "Life");
    else
        screen(screen_width, screen_height, false, "Life");

    buff_width = num_cells_x + 2;
    buff_height = num_cells_y + 2;
    buffer_1 = new bool[buff_width * buff_height];
    buffer_2 = new bool[buff_width * buff_height];
    current_state = buffer_1 + buff_width + 1;
    next_state = buffer_2 + buff_width + 1;
    init_cells(num_cells_x, num_cells_y, rand_percent);
    time = getTicks();
    sleep_to = time + ms_diff;

    // Main loop
    while (true) {
        if (simulate || step) {
            // Determine new_state per cell
            set_boundaries(num_cells_x, num_cells_y);
            for(int y = 0; y < num_cells_y; ++y) {
                for(int x = 0; x < num_cells_x; ++x) {
                    int num_neighbors = 0;
                    num_neighbors += get_current(x - 1, y - 1);
                    num_neighbors += get_current(x    , y - 1);
                    num_neighbors += get_current(x + 1, y - 1);
                    num_neighbors += get_current(x - 1, y    );
                    num_neighbors += get_current(x + 1, y    );
                    num_neighbors += get_current(x - 1, y + 1);
                    num_neighbors += get_current(x    , y + 1);
                    num_neighbors += get_current(x + 1, y + 1);

                    // Game definitions
                    switch (game) {
                    case SEEDS:
                        switch (num_neighbors) {
                        case 2:
                            set_next(x, y, !get_current(x, y));
                            break; 
                        default:   
                            set_next(x, y, 0);
                            break;  
                        }           
                        break;     

                    case BLOTCHES:
                        switch (num_neighbors) {
                        case 2:
                            set_next(x, y, rand() % 2);
                            break; 
                        default:   
                            set_next(x, y, get_current(x, y));
                            break;  
                        }           
                        break;     

                    case DIAMONDS:
                        switch (num_neighbors) {
                        case 2:
                            set_next(x, y, !get_current(x, y));
                            break; 
                        default:   
                            set_next(x, y, get_current(x, y));
                            break;  
                        }           
                        break;     

                    case DAY_AND_NIGHT:
                        switch (num_neighbors) {
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                            set_next(x, y, 1);
                            break; 
                        case 4:    
                            set_next(x, y, get_current(x, y));
                            break; 
                        default:   
                            set_next(x, y, 0);
                            break;
                        }
                        break;

                    case CONWAY:
                        switch (num_neighbors) {
                        case 2:
                            set_next(x, y, get_current(x, y));
                            break; 
                        case 3:    
                            set_next(x, y, 1);
                            break; 
                        default:   
                            set_next(x, y, 0);
                            break;
                        }
                        break;
                    }
                }
            }

            change_color(2);

            // Commit new_state
            bool * temp;
            temp = next_state;
            next_state = current_state;
            current_state = temp;
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
                        if (lmb)
                            set_current(x, y, 1);
                        else if (rmb)
                            set_current(x, y, 0);
                    }
                }
                else {
                    int increment = dy < 0 ? -1 : 1;
                    for(int y = last_clicked_cell_y; y != cell_y; y += increment) {
                        int x = last_clicked_cell_x + dx * (y - last_clicked_cell_y) / dy;
                        if (lmb)
                            set_current(x, y, 1);
                        else if (rmb)
                            set_current(x, y, 0);
                    }
                }
                if (lmb)
                    set_current(cell_x, cell_y, 1);
                else if (rmb)
                    set_current(cell_x, cell_y, 0);
            }
            last_clicked_cell_x = cell_x;
            last_clicked_cell_y = cell_y;
        }
        else {
            last_clicked_cell_x = -1;
            last_clicked_cell_y = -1;
        }

        readKeys();
        // Handle key presses
        {
            static bool pressed = false;
            if (keyDown(SDLK_1)) {
                if (!pressed) {
                    pressed = true;
                    if (game == CONWAY) {
                        game = SEEDS;
                        message = "SEEDS";
                    } else if (game == SEEDS) {
                        game = DIAMONDS;
                        message = "DIAMONDS";
                    } else if (game == DIAMONDS) {
                        game = BLOTCHES;
                        message = "BLOTCHES";
                    } else if (game == BLOTCHES) {
                        game = DAY_AND_NIGHT;
                        message = "DAY_AND_NIGHT";
                    } else { // game == DAY_AND_NIGHT
                        game = CONWAY;
                        message = "CONWAY";
                    }
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
                    pressed = true;
                    if (boundary == PACMAN) {
                        boundary = DEAD;
                        message = "DEAD BORDERS";
                    }
                    else if (boundary == DEAD) {
                        boundary = ALIVE;
                        message = "ALIVE BORDERS";
                    }
                    else if (boundary == ALIVE) {
                        boundary = PACMAN;
                        message = "PACMAN BORDERS";
                    }
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
            init_cells(num_cells_x, num_cells_y, rand_percent);
            end_print_time = getTicks() + 500;
            std::ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_RIGHT)) {
            rand_percent = 100;
            init_cells(num_cells_x, num_cells_y, rand_percent);
            end_print_time = getTicks() + 500;
            std::ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_PAGEDOWN) || keyDown(SDLK_DOWN)) {
            rand_percent = rand_percent == 0 ? 0 : rand_percent - 1;
            init_cells(num_cells_x, num_cells_y, rand_percent);
            end_print_time = getTicks() + 500;
            std::ostringstream convert;
            convert << rand_percent;
            message = convert.str() + "%";
        }
        if (keyDown(SDLK_PAGEUP) || keyDown(SDLK_UP)) {
            rand_percent = rand_percent == 100 ? 100 : rand_percent + 1;
            init_cells(num_cells_x, num_cells_y, rand_percent);
            end_print_time = getTicks() + 500;
            std::ostringstream convert;
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

        // Commit image of cells to buffer
        cls();
        if (do_color) {
            setColor(color);
        } else {
            setColor(white);
        }
        for (int cell_y = 0; cell_y < num_cells_y; ++cell_y) {
            for (int cell_x = 0; cell_x < num_cells_x; ++cell_x) {
                if (get_current(cell_x, cell_y)) {
                    int y_start = cell_y * screen_height / num_cells_y;
                    int x_start = cell_x * screen_width / num_cells_x;
                    int y_end = (cell_y + 1) * screen_height / num_cells_y - 1;
                    int x_end = (cell_x + 1) * screen_width / num_cells_x - 1;
                    drawRect(x_start, y_start, x_end, y_end);
                }
            }
        }

        old_time = time;
        time = getTicks();
#if 0
        // Diplay FPS
        print(1000.0 / (time - old_time));
#else
        // Display messages
        if (time < end_print_time)
            print(message);
#endif
        redraw(); // Draw SDL pixel buffer

        // Sleep so that FPS is maintained
        if (sleep_to > time) {
            SDL_Delay(sleep_to - time);
            sleep_to += ms_diff;
        }
        else {
            sleep_to += ((time - sleep_to)/ms_diff + 1) * ms_diff;
        }
    }
    delete [] buffer_1;
    delete [] buffer_2;
    return 0;
}

