#include "quickcg.h"
using namespace QuickCG;

#define FPS 60

struct cell
{
    char state;
    char new_state;
};

static struct
{
    int red;
    int green;
    int blue;
} color = {255, 0, 0};

static const int rate = 10;

static enum {UP_GREEN,
             DOWN_RED,
             UP_BLUE,
             DOWN_GREEN,
             UP_RED,
             DOWN_BLUE} state = UP_GREEN;

int main(int argc, char* argv[])
{
    int screenWidth;
    int screenHeight;
    int cellsX;
    int cellsY;
    bool simulate = false;
    int single_step = 0;

    if (argc == 1)
    {
        cellsX = 1366;
        cellsY = 768;
        screenWidth = 1366;
        screenHeight = 768;
    }
    else if (argc == 3)
    {
        cellsX = atoi(argv[1]);
        cellsY = atoi(argv[2]);
        screenWidth = 1366;
        screenHeight = 768;
    }
    else if (argc == 5)
    {
        cellsX = atoi(argv[1]);
        cellsY = atoi(argv[2]);
        screenWidth = atoi(argv[3]);
        screenHeight = atoi(argv[4]);
    }
    else
    {
        printf("Usage: ./life [Board_x] [Board_y] [Screen_x] [Screen_y]\n");
        exit(1);
    }
    if (cellsX == 0 || cellsY == 0 || screenWidth == 0 || screenHeight == 0)
    {
        printf("Usage: ./life [Board_x] [Board_y] [Screen_x] [Screen_y]\n");
        exit(1);
    }

    unsigned int buffer[screenWidth][screenHeight];
    cell cells[cellsX][cellsY];

    for(int x = 0; x < cellsX; ++x)
    {
        for(int y = 0; y < cellsY; ++y)
        {
            cells[x][y].state = rand() % 2;
        }
    }

    if (screenWidth == 1366 && screenHeight == 768)
        screen(screenWidth, screenHeight, 1, "Life");
    else
        screen(screenWidth, screenHeight, 0, "Life");

    float oldTime = getTime();
    while (!done(true,  false)) // Loop until Esc is pressed
    {
        waitFrame(oldTime, 1.0 / FPS);
        oldTime = getTime();
        // Commit to buffer
        for(int x = 0; x < screenWidth; ++x)
        {
            int cellx = x * cellsX / screenWidth;
            for(int y = 0; y < screenHeight; ++y)
            {
                int celly = y * cellsY / screenHeight;
                unsigned int c = (color.red << 16) | (color.blue << 8) | color.green;
                buffer[x][y] = c * cells[cellx][celly].state;
            }
        }

        // Draw buffer
        drawBuffer(*buffer); // Update SDL pixel buffer
        redraw(); // Draw SDL pixel buffer

        if (simulate || single_step)
        {
            single_step = 0;
            // Determine new_state per cell
            for(int x = 0; x < cellsX; ++x)
            {
                for(int y = 0; y < cellsY; ++y)
                {
                    int neighbors = 0;
                    if (x - 1 >= 0) 
                    {
                        if (y - 1 >= 0)
                            neighbors += cells[x-1][y-1].state;
                        neighbors += cells[x-1][y].state;
                        if (y + 1 < cellsY)
                            neighbors += cells[x-1][y+1].state;
                    }
                    if (y - 1 >= 0)
                        neighbors += cells[x][y-1].state;
                    if (y + 1 < cellsY)
                        neighbors += cells[x][y+1].state;
                    if (x + 1 < cellsX)
                    {
                        if (y - 1 >= 0)
                            neighbors += cells[x+1][y-1].state;
                        neighbors += cells[x+1][y].state;
                        if (y + 1 < cellsY)
                            neighbors += cells[x+1][y+1].state;
                    }

                    switch (neighbors)
                    {
                        case 2:
                            cells[x][y].new_state = cells[x][y].state;
                            break;
                        case 3:
                            cells[x][y].new_state = 1;
                            break;
                        default:
                            cells[x][y].new_state = 0;
                            break;
                    }
                }
            }

            // Change color
            switch (state)
            {
            case UP_GREEN:
                color.green += rate;
                if (color.green >= 255)
                {
                    color.green = 255;
                    state = DOWN_RED;
                }
                break;
            case DOWN_RED:
                color.red -= rate;
                if (color.red <= 0)
                {
                    color.red = 0;
                    state = UP_BLUE;
                }
                break;
            case UP_BLUE:
                color.blue += rate;
                if (color.blue >= 255)
                {
                    color.blue = 255;
                    state = DOWN_GREEN;
                }
                break;
            case DOWN_GREEN:
                color.green -= rate;
                if (color.green <= 0)
                {
                    color.green = 0;
                    state = UP_RED;
                }
                break;
            case UP_RED:
                color.red += rate;
                if (color.red >= 255)
                {
                    color.red = 255;
                    state = DOWN_BLUE;
                }
                break;
            case DOWN_BLUE:
                color.blue -= rate;
                if (color.blue <= 0)
                {
                    color.blue = 0;
                    state = UP_GREEN;
                }
                break;
            }

            // Commit new_state
            for(int x = 0; x < cellsX; ++x)
            {
                for(int y = 0; y < cellsY; ++y)
                {
                    cells[x][y].state = cells[x][y].new_state;
                }
            }
        }

        //readKeys();
        if (keyPressed(SDLK_F12))
        {
            simulate = !simulate;
        }
        if (keyPressed(SDLK_SPACE))
        {
            if (!simulate)
            {
                single_step = 1;
            }
        }
    }
    return 0;
}
