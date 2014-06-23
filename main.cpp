#include "quickcg.h"
using namespace QuickCG;

#define FPS 30

struct cell
{
    unsigned char state;
    unsigned char new_state;
};

static cell * cells;

static ColorRGB color(255, 0, 0);
static const ColorRGB black(0, 0, 0);
static const ColorRGB white(255, 255, 255);

static enum {UP_GREEN,   DOWN_RED, UP_BLUE,
             DOWN_GREEN, UP_RED,   DOWN_BLUE} color_state = UP_GREEN;
static enum {CONWAY, SEEDS, DIAMONDS, BLOTCHES, DAY_AND_NIGHT} game = CONWAY;
static enum {PACMAN, DEAD} boundary = PACMAN;

// Doesn't work for negative b
static int mod(int a, int b)
{
    int val = a % b;
    return (val >= 0) ? val : val + b;
}

static void change_color(int rate)
{
    switch (color_state)
    {
    case UP_GREEN:
        if ((int)color.g + rate >= 255)
        {
            color.g = 255;
            color_state = DOWN_RED;
        }
        else
            color.g += rate;
        break;
    case DOWN_RED:
        if ((int)color.r - rate <= 0)
        {
            color.r = 0;
            color_state = UP_BLUE;
        }
        else
            color.r -= rate;
        break;
    case UP_BLUE:
        if ((int)color.b + rate >= 255)
        {
            color.b = 255;
            color_state = DOWN_GREEN;
        }
        else
            color.b += rate;
        break;
    case DOWN_GREEN:
        if ((int)color.g - rate <= 0)
        {
            color.g = 0;
            color_state = UP_RED;
        }
        else
            color.g -= rate;
        break;
    case UP_RED:
        if ((int)color.r + rate >= 255)
        {
            color.r = 255;
            color_state = DOWN_BLUE;
        }
        else
            color.r += rate;
        break;
    case DOWN_BLUE:
        if ((int)color.b - rate <= 0)
        {
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

static void init_cells(int num_cells_x, int num_cells_y, int permil)
{
    for(int x = 0; x < num_cells_x; ++x)
    {
        for(int y = 0; y < num_cells_y; ++y)
        { 
            if (permil > rand() % 1000) cells[x * num_cells_y + y].state = 1;
            else cells[x * num_cells_y + y].state = 0;
        }
    }
}

int main(int argc, char* argv[])
{
    int screen_width;
    int screen_height;
    int num_cells_x;
    int num_cells_y;
    int single_step = 0;
    bool simulate = false;
    bool do_color = true;
    double old_time;
    double time = getTicks();

    srand(getTime());
    
    if (argc == 1)
    {
        num_cells_x = -1;
        num_cells_y = -1;
        screen_width = -1;
        screen_height = -1;
    }
    else if (argc == 3)
    {
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = -1;
        screen_height = -1;
    }
    else if (argc == 5)
    {
        num_cells_x = atoi(argv[1]);
        num_cells_y = atoi(argv[2]);
        screen_width = atoi(argv[3]);
        screen_height = atoi(argv[4]);
    }
    else
    {
        printf("Usage: ./life [Board_x Board_y] [Screen_x Screen_y]\n");
        exit(1);
    }
    if (num_cells_x == 0 || num_cells_y == 0 || screen_width == 0 || screen_height == 0)
    {
        printf("Usage: ./life [Board_x Board_y] [Screen_x Screen_y]\n");
        exit(1);
    }

    screen(screen_width, screen_height, "Life");

    if (screen_width < 0)
    {
        const SDL_VideoInfo * info = SDL_GetVideoInfo();
        screen_width = info->current_w;
        screen_height = info->current_h;
    }
    if (num_cells_x < 0)
    {
        num_cells_x = screen_width / 3;
        num_cells_y = screen_height / 3;
    }

    cells = new cell[num_cells_x * num_cells_y];

    init_cells(num_cells_x, num_cells_y, 500);

    while (!done(true, false)) // Loop until Esc is pressed
    {
        // Commit to buffer
        for(int x = 0; x < screen_width; ++x)
        {
            int cell_x = x * num_cells_x / screen_width;
            for(int y = 0; y < screen_height; ++y)
            {
                int cell_y = y * num_cells_y / screen_height;
                if (cells[cell_x * num_cells_y + cell_y].state)
                {
                    if (do_color)
                        pset(x, y, color);
                    else
                        pset(x, y, white);
                }
                else
                    pset(x, y, black);
            }
        }

        // Sleep so that FPS is maintained
        old_time = time;
        time = getTicks();
        int delay = (int)((1000.0 / FPS - (time - old_time)));
        if (delay > 0)
            SDL_Delay(delay);

        // Draw buffer
        //double frameTime = (time - old_time) / 1000.0; //frameTime is the time this frame has taken, in seconds
        //print(1.0 / frameTime); //FPS counter
        redraw(); // Draw SDL pixel buffer

        if (simulate || single_step)
        {
            single_step = 0;

            // Determine new_state per cell
            for(int x = 0; x < num_cells_x; ++x)
            {
                for(int y = 0; y < num_cells_y; ++y)
                {
                    int num_neighbors = 0;
                    
                    switch (boundary)
                    {
                    case PACMAN:
                        num_neighbors += cells[mod(x - 1, num_cells_x) * num_cells_y + mod(y - 1, num_cells_y)].state;
                        num_neighbors += cells[mod(x - 1, num_cells_x) * num_cells_y + mod(y    , num_cells_y)].state;
                        num_neighbors += cells[mod(x - 1, num_cells_x) * num_cells_y + mod(y + 1, num_cells_y)].state;
                        num_neighbors += cells[mod(x    , num_cells_x) * num_cells_y + mod(y - 1, num_cells_y)].state;
                        num_neighbors += cells[mod(x    , num_cells_x) * num_cells_y + mod(y + 1, num_cells_y)].state;
                        num_neighbors += cells[mod(x + 1, num_cells_x) * num_cells_y + mod(y - 1, num_cells_y)].state;
                        num_neighbors += cells[mod(x + 1, num_cells_x) * num_cells_y + mod(y    , num_cells_y)].state;
                        num_neighbors += cells[mod(x + 1, num_cells_x) * num_cells_y + mod(y + 1, num_cells_y)].state;
                        break;
                    case DEAD:
                        if (x - 1 >= 0) 
                        {
                            if (y - 1 >= 0)
                                num_neighbors += cells[(x - 1) * num_cells_y + y - 1].state;
                            num_neighbors += cells[(x - 1) * num_cells_y + y].state;
                            if (y + 1 < num_cells_y)
                                num_neighbors += cells[(x - 1) * num_cells_y + y + 1].state;
                        }
                        if (y - 1 >= 0)
                            num_neighbors += cells[x * num_cells_y + y - 1].state;
                        if (y + 1 < num_cells_y)
                            num_neighbors += cells[x * num_cells_y + y + 1].state;
                        if (x + 1 < num_cells_x)
                        {
                            if (y - 1 >= 0)
                                num_neighbors += cells[(x + 1) * num_cells_y + y - 1].state;
                            num_neighbors += cells[(x + 1) * num_cells_y + y].state;
                            if (y + 1 < num_cells_y)
                                num_neighbors += cells[(x + 1) * num_cells_y + y + 1].state;
                        }
                        break;
                    }

                    // Game definitions
                    switch (game)
                    {
                    case SEEDS:
                        switch (num_neighbors)
                        {
                        case 2:
                            cells[x * num_cells_y + y].new_state  = !cells[x * num_cells_y + y].state;
                            break;
                        default:
                            cells[x * num_cells_y + y].new_state = 0;
                            break;
                        }
                        break;

                    case BLOTCHES:
                        switch (num_neighbors)
                        {
                        case 2:
                            cells[x * num_cells_y + y].new_state  = rand() % 2;
                            break;
                        default:
                            cells[x * num_cells_y + y].new_state  = cells[x * num_cells_y + y].state;
                            break;
                        }
                        break;

                    case DIAMONDS:
                        switch (num_neighbors)
                        {
                        case 2:
                            cells[x * num_cells_y + y].new_state  = !cells[x * num_cells_y + y].state;
                            break;
                        default:
                            cells[x * num_cells_y + y].new_state  = cells[x * num_cells_y + y].state;
                            break;
                        }
                        break;

                    case DAY_AND_NIGHT:
                        switch (num_neighbors)
                        {
                        case 3:
                        case 6:
                        case 7:
                        case 8:
                            cells[x * num_cells_y + y].new_state  = 1;
                            break;
                        case 4:
                            cells[x * num_cells_y + y].new_state  = cells[x * num_cells_y + y].state;
                            break;
                        default:
                            cells[x * num_cells_y + y].new_state = 0;
                            break;
                        }
                        break;

                    case CONWAY:
                        switch (num_neighbors)
                        {
                        case 2:
                            cells[x * num_cells_y + y].new_state = cells[x * num_cells_y + y].state;
                            break;
                        case 3:
                            cells[x * num_cells_y + y].new_state  = 1;
                            break;
                        default:
                            cells[x * num_cells_y + y].new_state = 0;
                            break;
                        }
                        break;
                    }
                }
            }

            change_color(2);

            // Commit new_state
            for(int x = 0; x < num_cells_x; ++x)
            {
                for(int y = 0; y < num_cells_y; ++y)
                {
                    cells[x * num_cells_y + y].state = cells[x * num_cells_y + y].new_state;
                }
            }
        }

        // Handle key presses
        if (keyPressed(SDLK_F1))
            game = CONWAY;
        if (keyPressed(SDLK_F2))
            game = SEEDS;
        if (keyPressed(SDLK_F3))
            game = DIAMONDS;
        if (keyPressed(SDLK_F4))
            game = BLOTCHES;
        if (keyPressed(SDLK_F5))
            game = DAY_AND_NIGHT;
        if (keyPressed(SDLK_F6))
            do_color = !do_color;
        if (keyPressed(SDLK_F7))
            change_color(25);
        if (keyPressed(SDLK_F8))
            boundary = PACMAN;
        if (keyPressed(SDLK_F9))
            boundary = DEAD;
        if (keyPressed(SDLK_F10))
            init_cells(num_cells_x, num_cells_y, 500);
        if (keyPressed(SDLK_F11))
            init_cells(num_cells_x, num_cells_y, 2);
        if (keyPressed(SDLK_F12))
            simulate = !simulate;
        if (keyPressed(SDLK_SPACE))
        {
            if (!simulate)
                single_step = 1;
        }
    }
    delete [] cells;
    return 0;
}
