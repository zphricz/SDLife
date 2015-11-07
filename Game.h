#ifndef GAME_H
#define GAME_H

#include "Screen.h"

enum class GameType { CONWAY, SEEDS, GNARL, WALLED_CITIES, DAY_AND_NIGHT };
enum class BoundaryType { PACMAN, DEAD, ALIVE };
enum class ColorState {
    UP_GREEN,
    DOWN_RED,
    UP_BLUE,
    DOWN_GREEN,
    UP_RED,
    DOWN_BLUE
};

class Game {
  private:
    const int num_cells_x;
    const int num_cells_y;
    const int buff_width;
    const int buff_height;
    bool *const buffer_1;
    bool *const buffer_2;
    bool *current_state;
    bool *next_state;
    GameType game;
    BoundaryType boundary;
    ColorState color_state;
    SDL_Color color;
    PerfSoftScreen *const scr;
    bool show_fps;
    int rand_percent;
    bool step;
    bool simulate;
    bool do_color;
    bool running;
    bool clear_dead_cells;
    int image_number;

    // Games
    void conway(int x, int y);
    void seeds(int x, int y);
    void walled_cities(int x, int y);
    void gnarl(int x, int y);
    void day_and_night(int x, int y);

    void change_color(int rate);
    void draw_slice(int slice);
    void iterate_slice(int slice);
    int moore_neighbors(int x, int y);
    int extended_neighbors(int x, int y, int levels);
    bool &cell_at(int x, int y);
    bool &next_cell_at(int x, int y);
    void set_boundaries();
    void init_cells(int percent);
    void draw_cell(int x, int y);
    void draw_cells();
    void iterate();
    void switch_game();
    void switch_boundary();
    void handle_input();
    bool in_bounds(int x, int y);

    void loop();

  public:
    Game(int num_x, int num_y, PerfSoftScreen *screen);
    ~Game();
    void run();
};

#endif
