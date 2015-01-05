#ifndef GAME_H
#define GAME_H

#include <string>
using std::string;

typedef enum {CONWAY, SEEDS, DIAMONDS, BLOTCHES, DAY_AND_NIGHT} game_types;
typedef enum {PACMAN, DEAD, ALIVE} boundary_types;

class Game
{
    private:
        const int num_cells_x;
        const int num_cells_y;
        const int buff_width;
        const int buff_height;
        bool * const buffer_1;
        bool * const buffer_2;
        bool * current_state;
        bool * next_state;
        game_types game;
        boundary_types boundary;

        // Games
        bool conway(int num_neighbors, bool last);
        bool seeds(int num_neighbors, bool last);
        bool blotches(int num_neighbors, bool last);
        bool diamonds(int num_neighbors, bool last);
        bool day_and_night(int num_neighbors, bool last);

        bool& next_cell_at(int x, int y);
        void set_boundaries();

    public:
        Game(int num_x, int num_y);
        ~Game();
        int cells_x();
        int cells_y();
        bool& cell_at(int x, int y);
        void init_cells(int percent);
        void iterate();
        string switch_game();
        string switch_boundary();
};

#endif
