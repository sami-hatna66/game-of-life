#  The Game of Life

The Game of Life is a cellular automaton developed by mathematician John Conway. A cellular automaton is a collection of cells, each of which has a state which is determined by its own state and the state of its neighbouring cells. In the Game of Life there are four rules determining whether a cell is either alive or dead:

1. A live cell with less than two living neighbours dies (underpopulation)
2. A live cell with three or four live neighbours stays alive
3. A live cell with more than four live neighbours dies (overpopulation)
4. A dead cell with three live neighbours becomes a live cell (reproduction)

These four data-manipulation rules are Turing complete - meaning that anything that can be computed algorithmically can be computed within the universe of the Game of Life. Many mathematicians have catalogued numerous different patterns which occur in the Game of Life. These patterns can be categorised into still-lifes, oscillators and gliders.

My implementation of the Game of Life is written in C++, using SDL2 for the 2D graphics. main.cpp contains a function for running the Game of Life on a randomly generated grid of cells, as well as functions for creating three common patterns: Simkin's Glider Gun, Gosper's Puffer Train and Gosper's Glider Gun.

## Demo

https://user-images.githubusercontent.com/88731772/148647309-25ecf2e9-f702-4a63-b524-856868ee2866.mp4

***Note on border rules:*** *John Conway never specified what happens to cells at the border of the grid of cells as his theoretical model was supposed to take place in an infinite universe. However, because of the limitations of computers, there must be some kind of border to our grid. For my implementation, I decided to have the grid wraparound at each edge, although other implementations choose to just have all cells be dead.*

## Resources Used

- https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
- https://www.conwaylife.com/wiki/Main_Page
