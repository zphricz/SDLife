#ifndef GAME_H
#define GAME_H

#include "Screen.h"

typedef enum {CONWAY, SEEDS, GNARL, WALLED_CITIES, DAY_AND_NIGHT} GameTypeEnum;
typedef enum {PACMAN, DEAD, ALIVE} BoundaryTypeEnum;

class Game {
private:
    const int num_cells_x;
    const int num_cells_y;
    const int buff_width;
    const int buff_height;
    const int num_threads;
    bool * const buffer_1;
    bool * const buffer_2;
    bool * current_state;
    bool * next_state;
    GameTypeEnum game;
    BoundaryTypeEnum boundary;
    Screen * const scr;
    bool show_fps;
    int rand_percent;
    bool step;
    bool simulate;
    bool do_color;
    Uint32 fps_start_time;
    int fps_counter;
    bool running;
    bool clear_dead_cells;
    int image_number;

    // Games
    void conway(int x, int y);
    void seeds(int x, int y);
    void walled_cities(int x, int y);
    void gnarl(int x, int y);
    void day_and_night(int x, int y);

    static void thread_func(Game& self);
    static void process_slice(Game& self, int slice);
    int moore_neighbors(int x, int y);
    int extended_neighbors(int x, int y, int levels);
    bool& cell_at(int x, int y);
    bool& next_cell_at(int x, int y);
    void set_boundaries();
    void init_cells(int percent);
    void draw_cell(int x, int y);
    void draw_cells();
    void iterate();
    void switch_game();
    void switch_boundary();
    void handle_input();

public:
    Game(int num_x, int num_y, Screen * screen, int num_threads);
    ~Game();
    void run();
};

#endif
