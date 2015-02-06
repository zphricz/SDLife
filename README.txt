                     
                     LIFE-LIKE CELLULAR AUTOMATA SIMULATOR

Overview
--------

This program simulates life-like cellular automata with graphics rendered using
SDL2. The rendering is performed in software by the Screen class which acts as
a wrapper for SDL funtionality. To run the program, simply run:

    ./life

This will run the program in full screen with each simulated cell corresponding
to four pixels. You can specify the number of cells to simulate and the
resolution to render at with command line arguments:

    ./life [Board_x Board_y] [Screen_x Screen_y]

You can interact with the simulation  by pressing keys on your keyboard:

    Simulation modes
    ----------------
    Press 1 to switch between simulation modes, they iterate in the following
    order:
        - Conway's Game of Life
        - Seeds
        - Walled Cities
        - Gnarl
        - Day and Night

    Other Options
    -------------
    2: Switch boundary modes (PACMAN vs DEAD vs ALIVE)
    3: Flip drawing live cells in colors or in white
    4: Move colors ahead somewhat
    5: Switch between displaying FPS and not
    6: Switch between drawing dead cells and not
    7: Draw a .tga image of the current state of the screen
    0: Toggle between recording video and not
    left arrow: Initialize cells with all dead
    right arrow: initialize cells with all alive
    enter: Pause/start simulation
    space: Single-step simulation if paused
    PgUp/up arrow: Initialize cells with increasing percentage alive
    PgDn/down arrow: Initialize cells with decreasing percentage alive

You can also interact by using your mouse:

    LMB: Make a cell alive
    RMB: Make a cell dead

To close the program, press the escape key.

Authors
-------

Zack Hricz

