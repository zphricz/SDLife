#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "Game.h"

using std::swap;
using std::cout;
using std::endl;

// GAME DEFINITIONS
void Game::conway(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 2:
        next_cell_at(x, y) = cell_at(x, y);
        break;
    case 3:    
        next_cell_at(x, y) = 1;
        break;
    default:   
        next_cell_at(x, y) = 0;
        break;
    }
}

void Game::seeds(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 2:
        next_cell_at(x, y) = !cell_at(x, y);
        break;
    default:   
        next_cell_at(x, y) = 0;
        break;
    }
}

void Game::blotches(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 2:
        next_cell_at(x, y) = rand() % 2;
        break;
    default:   
        next_cell_at(x, y) = cell_at(x, y);
        break;
    }
}

/*void Game::blotches(int x, int y) {
    //int num_neighbors = moore_neighbors(x, y);
    int num_neighbors = extended_neighbors(x, y, 2);
    switch (num_neighbors) {
    case 3:
        next_cell_at(x, y) = cell_at(x, y) || (rand() % 100 < 50);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        next_cell_at(x, y) = 1;
        break;
    default:   
        next_cell_at(x, y) = cell_at(x, y);
        break;
    }
}*/

void Game::diamonds(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 2:
        next_cell_at(x, y) = !cell_at(x, y);
        break;
    default:   
        next_cell_at(x, y) = cell_at(x, y);
        break;
    }
}

void Game::day_and_night(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 3:
    case 6:
    case 7:
    case 8:
        next_cell_at(x, y) = 1;
        break;
    case 4:    
        next_cell_at(x, y) = cell_at(x, y);
        break;
    default:   
        next_cell_at(x, y) = 0;
        break;
    }
}

Game::Game(int num_x, int num_y, Screen * screen) :
    num_cells_x(num_x),
    num_cells_y(num_y),
    buff_width(num_x + 2),
    buff_height(num_y + 2),
    buffer_1(new bool[buff_width * buff_height]),
    buffer_2(new bool[buff_width * buff_height]),
    scr(screen) {
    current_state = buffer_1 + buff_width + 1;
    next_state = buffer_2 + buff_width + 1;
    game = DAY_AND_NIGHT;
    boundary = PACMAN;
}

Game::~Game() {
    delete buffer_1;
    delete buffer_2;
}

bool& Game::cell_at(int x, int y) {
#if 0
    int index = (y + 1) * buff_width + (x + 1);
    if (index >= buff_width * buff_height || index < 0) {
        cout << "X: " << x << endl
             << "Y: " << y << endl
             << "INDEX: " << index << endl
             << "WIDTH: " << buff_width << endl
             << "HEIGHT: " << buff_height << endl
             << "WIDTH * HEGIHT: " << buff_width * buff_height << endl;
        exit(1);
    }
#endif
    return current_state[y * buff_width + x];
}

bool& Game::next_cell_at(int x, int y) {
#if 0
    int index = (y + 1) * buff_width + (x + 1);
    if (index >= buff_width * buff_height || index < 0) {
        cout << "X: " << x << endl
             << "Y: " << y << endl
             << "INDEX: " << index << endl
             << "WIDTH: " << buff_width << endl
             << "HEIGHT: " << buff_height << endl
             << "WIDTH * HEGIHT: " << buff_width * buff_height << endl;
        exit(1);
    }
#endif
    return next_state[y * buff_width + x];
}

int Game::moore_neighbors(int x, int y) {
    int num_neighbors = 0;
    num_neighbors += cell_at(x - 1, y - 1);
    num_neighbors += cell_at(x    , y - 1);
    num_neighbors += cell_at(x + 1, y - 1);
    num_neighbors += cell_at(x - 1, y    );
    num_neighbors += cell_at(x + 1, y    );
    num_neighbors += cell_at(x - 1, y + 1);
    num_neighbors += cell_at(x    , y + 1);
    num_neighbors += cell_at(x + 1, y + 1);
    return num_neighbors;
}

int Game::extended_neighbors(int x, int y, int levels) {
    int num_neighbors = 0;
    for (int j = y - levels; j <= y + levels; ++j) {
        for (int i = x - levels; i <= x + levels; ++i) {
            if (i == x && j == y) {
                continue;
            }
            if (i > num_cells_x) {
                continue;
            }
            if (i < -1) {
                continue;
            }
            if (j > num_cells_y) {
                continue;
            }
            if (j < -1) {
                continue;
            }
            num_neighbors += cell_at(i, j);
        }
    }
    return num_neighbors;
}

void Game::set_boundaries() {
    switch(boundary) {
    case PACMAN:
        cell_at(-1, -1) = cell_at(num_cells_x - 1, num_cells_y - 1);
        cell_at(num_cells_x, -1) = cell_at(0, num_cells_y - 1);
        cell_at(-1, num_cells_y) = cell_at(num_cells_x - 1, 0);
        cell_at(num_cells_x, num_cells_y) = cell_at(0, 0);
        for (int x = 0; x < num_cells_x; ++x) {
            cell_at(x, -1) = cell_at(x, num_cells_y - 1);
            cell_at(x, num_cells_y) = cell_at(x, 0);
        }
        for (int y = 0; y < num_cells_y; ++y) {
            cell_at(-1, y) = cell_at(num_cells_x - 1, y);
            cell_at(num_cells_x, y) = cell_at(0, y);
        }
        break;
    case ALIVE:
        cell_at(-1, -1) = 1;
        cell_at(num_cells_x, -1) = 1;
        cell_at(-1, num_cells_y) = 1;
        cell_at(num_cells_x, num_cells_y) = 1;
        for (int x = 0; x < num_cells_x; ++x) {
            cell_at(x, -1) = 1;
            cell_at(x, num_cells_y) = 1;
        }
        for (int y = 0; y < num_cells_y; ++y) {
            cell_at(-1, y) = 1;
            cell_at(num_cells_x, y) = 1;
        }
        break;
    case DEAD:
        cell_at(-1, -1) = 0;
        cell_at(num_cells_x, -1) = 0;
        cell_at(-1, num_cells_y) = 0;
        cell_at(num_cells_x, num_cells_y) = 0;
        for (int x = 0; x < num_cells_x; ++x) {
            cell_at(x, -1) = 0;
            cell_at(x, num_cells_y) = 0;
        }
        for (int y = 0; y < num_cells_y; ++y) {
            cell_at(-1, y) = 0;
            cell_at(num_cells_x, y) = 0;
        }
        break;
    default:
        break;
    }
}

void Game::init_cells(int percent) {
    for(int y = 0; y < num_cells_y; ++y) { 
        for(int x = 0; x < num_cells_x; ++x) {
            if (percent > rand() % 100) cell_at(x, y) = 1;
            else cell_at(x, y) = 0;
        }
    }
}

void Game::iterate() {
    set_boundaries();
    const game_types hoisted_game = game;
    for(int y = 0; y < num_cells_y; ++y) {
        for(int x = 0; x < num_cells_x; ++x) {
            switch (hoisted_game) {
            case CONWAY:
                conway(x, y);
                break;
            case SEEDS:
                seeds(x, y);
                break;
            case DIAMONDS:
                diamonds(x, y);
                break;
            case BLOTCHES:
                blotches(x, y);
                break;
            case DAY_AND_NIGHT:
                day_and_night(x, y);
                break;
            default:
                break;
            }
            if (cell_at(x, y)) {
                draw_cell(x, y);
            }
        }
    }
    // Commit new_state
    swap(next_state, current_state);
}

void Game::draw_cell(int x, int y) {
    int y_start = y * scr->height / num_cells_y;
    int x_start = x * scr->width / num_cells_x;
    int y_end = (y + 1) * scr->height / num_cells_y - 1;
    int x_end = (x + 1) * scr->width / num_cells_x - 1;
    scr->drawRect(x_start, y_start, x_end, y_end);
}

void Game::draw_cells() {
    for(int y = 0; y < num_cells_y; ++y) {
        for(int x = 0; x < num_cells_x; ++x) {
            if (cell_at(x, y)) {
                draw_cell(x, y);
            }
        }
    }
}

string Game::switch_game() {
    switch (game) {
    case CONWAY:
        game = SEEDS;
        return "SEEDS";
    case SEEDS:
        game = DIAMONDS;
        return "DIAMONDS";
    case DIAMONDS:
        game = BLOTCHES;
        return "BLOTCHES";
    case BLOTCHES:
        game = DAY_AND_NIGHT;
        return "DAY_AND_NIGHT";
    case DAY_AND_NIGHT:
        game = CONWAY;
        return "CONWAY";
    default:
        game = CONWAY;
        return "CONWAY";
    }
}

string Game::switch_boundary() {
    switch (boundary) {
    case PACMAN:
        boundary = DEAD;
        return "DEAD BORDERS";
    case DEAD:
        boundary = ALIVE;
        return "ALIVE BORDERS";
    case ALIVE:
        boundary = PACMAN;
        return "PACMAN BORDERS";
    default:
        boundary = PACMAN;
        return "PACMAN BORDERS";
    }
}
