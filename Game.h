#ifndef GAME_H
#define GAME_H

#include <string>
#include "Screen.h"

typedef enum {CONWAY, SEEDS, DIAMONDS, BLOTCHES, DAY_AND_NIGHT} GameTypeEnum;
typedef enum {PACMAN, DEAD, ALIVE} BoundaryTypeEnum;

class Game {
private:
    const int buff_width;
    const int buff_height;
    bool * const buffer_1;
    bool * const buffer_2;
    bool * current_state;
    bool * next_state;
    GameTypeEnum game;
    BoundaryTypeEnum boundary;
    Screen * const scr;

    // Games
    void conway(int x, int y);
    void seeds(int x, int y);
    void blotches(int x, int y);
    void diamonds(int x, int y);
    void day_and_night(int x, int y);

    static void process_slice(Game& self, int slice);
    int moore_neighbors(int x, int y);
    int extended_neighbors(int x, int y, int levels);
    bool& next_cell_at(int x, int y);
    void set_boundaries();

public:
    const int num_cells_x;
    const int num_cells_y;
    Game(int num_x, int num_y, Screen * screen);
    ~Game();
    bool& cell_at(int x, int y);
    void init_cells(int percent);
    void draw_cell(int x, int y);
    void draw_cells();
    void iterate();
    std::string switch_game();
    std::string switch_boundary();
};

#endif
