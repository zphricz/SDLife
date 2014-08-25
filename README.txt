                     
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
    F1: Change to Conway's Game of Life
    F2: Change to Seeds
    F3: Change to Diamonds
    F4: Change to Blotches
    F5: Change to Day and Night

    Other Options
    -------------
    F6: Flip drawing live cells in colors or in white
    F7: Move colors ahead somewhat
    F8: Set boundary mode to pacman style
    F9: Set boundary mode to dead-cell style
    F10: Initialize cells with 50% alive
    F11: initialize cells with 0.2% alive
    F12: Pause/start simulation
    Space: Single-step simulation if paused

You can also interact by using your mouse:

    LMB: Make a cell alive
    RMB: Make a cell dead
    Both: Toggle a cell state

Authors
-------

The logic begind the cellular automata was written by me, Zack Hricz. The wrapper
functions found in quickcg.h and quickcg.c were written by Lode Vadevenne.

