                     
                     LIFE-LIKE CELLULAR AUTOMATA SIMULATOR

Overview
--------

This program simulates life-like cellular automata with graphics rendered using
SDL. Much of the SDL code is wrapped by some convenience functions found in 
quickcg.h and quickcg.c. To run the program, simply run:

    ./life

This will run the program in full screen with each simulated cell corresponding
to four pixels. You can specify the number of cells to simulate and the
resolution to render at with command line arguments:

    ./life [Board_x Board_y] [Screen_x Screen_y]

You can interact with the project by pressing keys on your keyboard:

    Simulation modes
    ----------------
    Press 1 to switch between simulation modes, they iterate in the following
    order:
        - Conway's Game of Life
        - Seeds
        - Diamonds
        - Blotches
        - Day and Night

    Other Options
    -------------
    2: Switch boundary modes (PACMAND vs DEAD vs ALIVE)
    3: Flip drawing live cells in colors or in white
    4: Move colors ahead somewhat
    5: Switch between displaying FPS and not
    left arrow: Initialize cells with all dead
    right arrow: initialize cells with all alive
    enter: Pause/start simulation
    space: Single-step simulation if paused
    PgUp/up arrow: Initialize cells with increasing percentage alive
    PgDn/down arrow: Initialize cells with decreasing percentage alive

You can also interact by using your mouse:

    LMB: Make a cell alive
    RMB: Make a cell dead

Authors
-------

The logic begind the cellular automata was written by me, Zack Hricz. The
wrapper functions found in quickcg.h and quickcg.c were written by Lode
Vadevenne and modified by Zack Hricz.

