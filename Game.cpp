#include<stdlib.h>
#include "Game.h"

// GAME DEFINITIONS
bool Game::conway(int num_neighbors, bool last) {
    switch (num_neighbors) {
    case 2:
        return last;
    case 3:    
        return 1;
    default:   
        return 0;
    }
}

bool Game::seeds(int num_neighbors, bool last) {
    switch (num_neighbors) {
    case 2:
        return !last;
    default:   
        return 0;
    }
}

bool Game::blotches(int num_neighbors, bool last) {
    switch (num_neighbors) {
    case 2:
        return rand() % 2;
    default:   
        return last;
    }
}

bool Game::diamonds(int num_neighbors, bool last) {
    switch (num_neighbors) {
    case 2:
        return !last;
    default:   
        return last;
    }
}

bool Game::day_and_night(int num_neighbors, bool last) {
    switch (num_neighbors) {
    case 3:
    case 6:
    case 7:
    case 8:
        return 1;
    case 4:    
        return last;
    default:   
        return 0;
    }
}

Game::Game(int num_x, int num_y) :
    num_cells_x(num_x),
    num_cells_y(num_y),
    buff_width(num_x + 2),
    buff_height(num_y + 2),
    buffer_1(new bool[buff_width * buff_height]),
    buffer_2(new bool[buff_width * buff_height]) {
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
    return current_state[y * buff_width + x];
}

bool& Game::next_cell_at(int x, int y) {
    return next_state[y * buff_width + x];
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

int Game::cells_x() {
    return num_cells_x;
}

int Game::cells_y() {
    return num_cells_y;
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
            int num_neighbors = 0;
            num_neighbors += cell_at(x - 1, y - 1);
            num_neighbors += cell_at(x    , y - 1);
            num_neighbors += cell_at(x + 1, y - 1);
            num_neighbors += cell_at(x - 1, y    );
            num_neighbors += cell_at(x + 1, y    );
            num_neighbors += cell_at(x - 1, y + 1);
            num_neighbors += cell_at(x    , y + 1);
            num_neighbors += cell_at(x + 1, y + 1);
            switch (hoisted_game) {
            case CONWAY:
                next_cell_at(x, y) = conway(num_neighbors, cell_at(x, y));
                break;
            case SEEDS:
                next_cell_at(x, y) = seeds(num_neighbors, cell_at(x, y));
                break;
            case DIAMONDS:
                next_cell_at(x, y) = diamonds(num_neighbors, cell_at(x, y));
                break;
            case BLOTCHES:
                next_cell_at(x, y) = blotches(num_neighbors, cell_at(x, y));
                break;
            case DAY_AND_NIGHT:
                next_cell_at(x, y) = day_and_night(num_neighbors, cell_at(x, y));
                break;
            default:
                break;
            }
        }
    }
    // Commit new_state
    bool * temp;
    temp = next_state;
    next_state = current_state;
    current_state = temp;
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

