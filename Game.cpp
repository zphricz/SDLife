#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <vector>
#include <future>
#include "Game.h"

using namespace std;

void Game::change_color(int rate) {
    switch (color_state) {
    case ColorState::UP_GREEN:
        if ((int)color.g + rate >= 255) {
            color.g = 255;
            color_state = ColorState::DOWN_RED;
        } else
            color.g += rate;
        break;
    case ColorState::DOWN_RED:
        if ((int)color.r - rate <= 0) {
            color.r = 0;
            color_state = ColorState::UP_BLUE;
        } else
            color.r -= rate;
        break;
    case ColorState::UP_BLUE:
        if ((int)color.b + rate >= 255) {
            color.b = 255;
            color_state = ColorState::DOWN_GREEN;
        } else
            color.b += rate;
        break;
    case ColorState::DOWN_GREEN:
        if ((int)color.g - rate <= 0) {
            color.g = 0;
            color_state = ColorState::UP_RED;
        } else
            color.g -= rate;
        break;
    case ColorState::UP_RED:
        if ((int)color.r + rate >= 255) {
            color.r = 255;
            color_state = ColorState::DOWN_BLUE;
        } else
            color.r += rate;
        break;
    case ColorState::DOWN_BLUE:
        if ((int)color.b - rate <= 0) {
            color.b = 0;
            color_state = ColorState::UP_GREEN;
        } else
            color.b -= rate;
        break;
    default:
        break;
    }
}

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

void Game::walled_cities(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 2:
    case 3:
    case 7:
    case 8:
        next_cell_at(x, y) = cell_at(x, y);
        break;
    case 4:
    case 5:
        next_cell_at(x, y) = 1;
        break;
    default:
        next_cell_at(x, y) = 0;
        break;
    }
}

void Game::gnarl(int x, int y) {
    int num_neighbors = moore_neighbors(x, y);
    switch (num_neighbors) {
    case 1:
        next_cell_at(x, y) = 1;
        break;
    default:
        next_cell_at(x, y) = 0;
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

Game::Game(int num_x, int num_y, PerfSoftScreen * screen, int num_threads) :
    num_cells_x(num_x),
    num_cells_y(num_y),
    buff_width(num_x + 2),
    buff_height(num_y + 2),
    num_threads(num_threads), 
    buffer_1(new bool[buff_width * buff_height]),
    buffer_2(new bool[buff_width * buff_height]),
    scr(screen) {
    current_state = buffer_1 + buff_width + 1;
    next_state = buffer_2 + buff_width + 1;
    game = GameType::DAY_AND_NIGHT;
    boundary = BoundaryType::PACMAN;
    color_state = ColorState::UP_GREEN;
    color = {255, 0, 0};
    show_fps = true;
    rand_percent = 50;
    step = false;
    simulate = false;
    do_color = true;
    running = true;
    clear_dead_cells = true;
    scr->set_recording_style("images", 5);
}

Game::~Game() {
    delete [] buffer_1;
    delete [] buffer_2;
}

bool& Game::cell_at(int x, int y) {
    return current_state[y * buff_width + x];
}

bool& Game::next_cell_at(int x, int y) {
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
    case BoundaryType::PACMAN:
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
    case BoundaryType::ALIVE:
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
    case BoundaryType::DEAD:
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

void Game::iterate_slice(int slice) {
    const GameType hoisted_game = game;
    int start = slice * num_cells_y / num_threads;
    int end = (slice + 1) * num_cells_y / num_threads;
    for (int y = start; y < end; y++) {
        for(int x = 0; x < num_cells_x; ++x) {
            switch (hoisted_game) {
            case GameType::CONWAY:
                conway(x, y);
                break;
            case GameType::SEEDS:
                seeds(x, y);
                break;
            case GameType::GNARL:
                gnarl(x, y);
                break;
            case GameType::WALLED_CITIES:
                walled_cities(x, y);
                break;
            case GameType::DAY_AND_NIGHT:
                day_and_night(x, y);
                break;
            default:
                break;
            }
        }
    }
}

void Game::iterate() {
    set_boundaries();
    vector<future<void>> futures;
    futures.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back(async(launch::async, &Game::iterate_slice, this, i));
    }
    for (auto& f: futures) {
        f.get();
    }
    swap(next_state, current_state); // Commit new_state
}

void Game::draw_cell(int x, int y) {
    int y_start = y * scr->height / num_cells_y;
    int x_start = x * scr->width / num_cells_x;
    int y_end = (y + 1) * scr->height / num_cells_y - 1;
    int x_end = (x + 1) * scr->width / num_cells_x - 1;
    if (y_end < y_start || x_end < x_start) {
        return;
    }
    scr->fill_rect(x_start, y_start, x_end, y_end);
}

void Game::draw_slice(int slice) {
    int start = slice * num_cells_y / num_threads;
    int end = (slice + 1) * num_cells_y / num_threads;
    for(int y = start; y < end; ++y) {
        for(int x = 0; x < num_cells_x; ++x) {
            if (cell_at(x, y)) {
                draw_cell(x, y);
            }
        }
    }
}

void Game::draw_cells() {
    vector<future<void>> futures;
    futures.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back(async(launch::async, &Game::draw_slice, this, i));
    }
    for (auto& f: futures) {
        f.get();
    }
}

void Game::switch_game() {
    switch (game) {
    case GameType::CONWAY:
        game = GameType::SEEDS;
        cout << "SEEDS" << endl;
        break;
    case GameType::SEEDS:
        game = GameType::GNARL;
        cout << "GNARL" << endl;
        break;
    case GameType::GNARL:
        game = GameType::WALLED_CITIES;
        cout << "WALLED CITIES" << endl;
        break;
    case GameType::WALLED_CITIES:
        game = GameType::DAY_AND_NIGHT;
        cout << "DAY AND NIGHT" << endl;
        break;
    case GameType::DAY_AND_NIGHT:
        game = GameType::CONWAY;
        cout << "CONWAY" << endl;
        break;
    default:
        game = GameType::CONWAY;
        cout << "CONWAY" << endl;
        break;
    }
}

void Game::switch_boundary() {
    switch (boundary) {
    case BoundaryType::PACMAN:
        boundary = BoundaryType::DEAD;
        cout << "DEAD BORDERS" << endl;
        break;
    case BoundaryType::DEAD:
        boundary = BoundaryType::ALIVE;
        cout << "ALIVE BORDERS" << endl;
        break;
    case BoundaryType::ALIVE:
        boundary = BoundaryType::PACMAN;
        cout << "PACMAN BORDERS" << endl;
        break;
    default:
        boundary = BoundaryType::PACMAN;
        cout << "PACMAN BORDERS" << endl;
        break;
    }
}

void Game::handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            running = false;
            break;
        }
        case SDL_KEYDOWN: {
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE: {
                running = false;
                break;
            }
            case SDLK_RETURN: {
                simulate = !simulate;
                if (simulate) {
                    cout << "STARTING SIMULATION" << endl;
                } else {
                    cout << "STOPPING SIMULATION" << endl;
                }
                break;
            }
            case SDLK_SPACE: {
                step = true;
                if (!simulate) {
                    cout << "SINGLE STEP" << endl;
                }
                break;
            }
            case SDLK_1: {
                switch_game();
                break;
            }
            case SDLK_2: {
                switch_boundary();
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
            case SDLK_6: {
                clear_dead_cells = !clear_dead_cells;
                break;
            }
            case SDLK_7: {
                ostringstream convert;
                convert << image_number;
                string name = "image_" + convert.str() + ".tga";
                scr->write_tga(name.c_str());
                image_number++;
                break;
            }
            case SDLK_0: {
                scr->toggle_recording();
                break;
            }
            case SDLK_LEFT: {
                rand_percent = 0;
                init_cells(rand_percent);
                cout << "RAND_PERCENT: " << rand_percent << "%" << endl;
                break;
            }
            case SDLK_RIGHT: {
                rand_percent = 100;
                init_cells(rand_percent);
                cout << "RAND_PERCENT: " << rand_percent << "%" << endl;
                break;
            }
            case SDLK_UP: {
                if (rand_percent < 100) {
                    rand_percent++;
                }
                init_cells(rand_percent);
                cout << "RAND_PERCENT: " << rand_percent << "%" << endl;
                break;
            }
            case SDLK_DOWN: {
                if (rand_percent > 0) {
                    rand_percent--;
                }
                init_cells(rand_percent);
                cout << "RAND_PERCENT: " << rand_percent << "%" << endl;
                break;
            }
            default: {
                break;
            }
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            int x = num_cells_x * event.button.x / scr->width;
            int y = num_cells_y * event.button.y / scr->height;
            if (event.button.button == SDL_BUTTON_LEFT) {
                cell_at(x, y) = 1;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                cell_at(x, y) = 0;
            }
            break;
        }
        case SDL_MOUSEMOTION: {
            int end_x = event.motion.x;
            int end_y = event.motion.y;
            int start_x = end_x - event.motion.xrel;
            int start_y = end_y - event.motion.yrel;
            int start_cell_x = start_x * num_cells_x / scr->width;
            int start_cell_y = start_y * num_cells_y / scr->height;
            int end_cell_x = end_x * num_cells_x / scr->width;
            int end_cell_y = end_y * num_cells_y / scr->height;
            int dx = end_cell_x - start_cell_x;
            int dy = end_cell_y - start_cell_y;
            bool left_click = event.motion.state & SDL_BUTTON_LMASK;
            bool right_click = event.motion.state & SDL_BUTTON_RMASK;
            if (!left_click && !right_click) {
                break;
            }
            if (abs(dx) > abs(dy)) {
                int increment = dx < 0 ? -1 : 1;
                for(int x = start_cell_x; x != end_cell_x; x += increment) {
                    int y = start_cell_y + dy * (x - start_cell_x) / dx;
                    if (left_click) {
                        cell_at(x, y) = 1;
                    } else {
                        cell_at(x, y) = 0;
                    }
                }
            } else {
                int increment = dy < 0 ? -1 : 1;
                for(int y = start_cell_y; y != end_cell_y; y += increment) {
                    int x = start_cell_x + dx * (y - start_cell_y) / dy;
                    if (left_click) {
                        cell_at(x, y) = 1;
                    } else {
                        cell_at(x, y) = 0;
                    }
                }
            }
            if (left_click) {
                cell_at(end_cell_x, end_cell_y) = 1;
            } else {
                cell_at(end_cell_x, end_cell_y) = 0;
            }
            break;
        }
        default: {
             break;
        }
        }
    }
}


void Game::run() {
    init_cells(rand_percent);
    cout << "DAY AND NIGHT" << endl;
    int fps_counter = 0;

    // Main loop
    while (running) {
        if (do_color) {
            scr->set_color(color);
        } else {
            scr->set_color(255, 255, 255);
        }
        if (simulate || step) {
            iterate();
            change_color(2);
            step = false;
        } else if (scr->is_direct_draw()) {
            // Wait for a bit so that the renderer can finish drawing
            SDL_Delay(2);
        }
        if (fps_counter++ == 10) {
            if (show_fps) {
                cout << "FPS: " << scr->fps() << endl;
            }
            fps_counter = 0;
        }
        if (clear_dead_cells) {
            scr->cls();
        }
        draw_cells();
        handle_input();
        scr->commit(); // Draw SDL pixel buffer
    }
}

