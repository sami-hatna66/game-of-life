//
//  main.cpp
//  game-of-life
//
//  Created by Sami Hatna on 06/01/2022.
//

#include <SDL2/SDL.h>
#include <array>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

const int NUMTHREADS = std::thread::hardware_concurrency();
constexpr const int MAPWIDTH = 170; // num of cells along x
constexpr const int MAPHEIGHT = 160; // num of cells along y
constexpr const int SCALE = 3; // size of individual cell
constexpr const int DELAY = 5000; // time between each lifecycle
constexpr const int CHANCEOFLIFE = 25; // probability of a cell being alive when doing random init

// Fill window with random living cells
void initRandomMap(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    std::random_device rd;
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distr(0, 100);
            int result = distr(gen);
            map[i][j] = (result < CHANCEOFLIFE);
        }
    }
}

// create empty map and populate with Simkin's glider gun pattern (2015)
void initSimkinGliderGun(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    int gunCoords[31][2] = {
        {47, 41}, {47, 42}, {48, 41}, {48, 42}, {47, 48},
        {47, 49}, {48, 48}, {48, 49}, {50, 46}, {50, 45},
        {51, 46}, {51, 45}, {57, 53}, {58, 53}, {59, 53},
        {58, 54}, {58, 55}, {57, 55}, {56, 55}, {56, 68},
        {56, 69}, {55, 68}, {55, 69}, {58, 66}, {58, 65},
        {59, 66}, {59, 65}, {58, 72}, {58, 73}, {59, 72},
        {59, 73}
    };
    
    for (int k = 0; k < 31; k++) {
        map[gunCoords[k][0]][gunCoords[k][1]] = 1;
    }
}

// create empty map and populate with Gosper's glider gun pattern (1970)
void initGosperGliderGun(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    int gunCoords[45][2] = {
        {56, 42}, {56, 43}, {57, 42}, {57, 43}, {56, 53},
        {55, 53}, {57, 53}, {54, 54}, {58, 54}, {53, 55},
        {59, 55}, {54, 56}, {58, 56}, {56, 57}, {55, 57},
        {57, 57}, {56, 58}, {55, 58}, {57, 58}, {55, 63},
        {54, 63}, {53, 63}, {53, 64}, {52, 64}, {55, 64},
        {56, 64}, {53, 65}, {52, 65}, {55, 65}, {56, 65},
        {52, 66}, {53, 66}, {54, 66}, {55, 66}, {56, 66},
        {52, 67}, {51, 67}, {56, 67}, {57, 67}, {52, 72},
        {53, 72}, {54, 76}, {55, 76}, {54, 77}, {55, 77}
    };
    
    for (int k = 0; k < 45; k++) {
        map[gunCoords[k][0]][gunCoords[k][1]] = 1;
    }
}

// create empty map and populate with Gosper's Puffer 1 pattern (1971)
void initPufferTrain(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    int pufferCoords[44][2] = {
        {42, 2}, {43, 3}, {43, 4}, {43, 5}, {43, 6},
        {43, 7}, {43, 8}, {42, 8}, {41, 8}, {40, 7},
        {46, 2}, {46, 3}, {46, 4}, {47, 2}, {47, 3},
        {48, 6}, {49, 6}, {51, 6}, {51, 7}, {50, 7},
        {49, 7}, {50, 8}, {55, 6}, {57, 6}, {58, 6},
        {55, 7}, {56, 7}, {57, 7}, {56, 8}, {59, 2},
        {60, 2}, {59, 3}, {60, 3}, {60, 4}, {64, 2},
        {63, 3}, {63, 4}, {63, 5}, {63, 6}, {63, 7},
        {63, 8}, {64, 8}, {65, 8}, {66, 7}
    };
    
    for (int k = 0; k < 44; k++) {
        map[pufferCoords[k][0]][pufferCoords[k][1]] = 1;
    }
}

void lifeKernel(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map,
                const std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &copy,
                const int startRow, const int endRow) {
    for (int row = startRow; row < endRow; row++) {
        for (int col = 0; col < MAPWIDTH; col++) {
            int liveNeighbourCount = 0;
            int neighbourCoords[8][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
            for (auto coord : neighbourCoords) {
                coord[0] = (row + coord[0]) % MAPHEIGHT;
                coord[1] = (col + coord[1]) % MAPWIDTH;
                liveNeighbourCount += copy[coord[0]][coord[1]];
            }

            if (map[row][col] && (liveNeighbourCount < 2 || liveNeighbourCount > 3)) {
                map[row][col] = false;
            } else if (liveNeighbourCount == 3) {
                map[row][col] = true;
            }
        }
    }
}

// One pass in the cellular lifecycle
void lifePass(std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> copy = map;

    const int rowsPerThread = (map.size() + NUMTHREADS - 1) / NUMTHREADS;

    std::vector<std::thread> threads;
    threads.reserve(NUMTHREADS);

    for (int i = 0; i < NUMTHREADS; i++) {
        const int startRow = i * rowsPerThread;
        const int endRow = std::min((i + 1) * rowsPerThread, (int)map.size());
        threads.push_back(std::thread(lifeKernel, std::ref(map),
                                      std::cref(copy), startRow,
                                      endRow));
    }

    for (int i = 0; i < NUMTHREADS; i++) {
        threads[i].join();
    }
}

// Initialise SDL components
void init(SDL_Window *&win, SDL_Renderer *&render) {
    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, MAPWIDTH * SCALE,
                           MAPHEIGHT * SCALE, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
}

// Draw frame
void draw(SDL_Renderer *&render, const std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> &map) {
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderClear(render);

    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_Rect r;
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (map[i][j]) {
                r.x = j * SCALE;
                r.y = i * SCALE;
                r.w = SCALE;
                r.h = SCALE;
                SDL_RenderFillRect(render, &r);
            }
        }
    }

    SDL_RenderPresent(render);
}

void close(SDL_Window *win, SDL_Renderer *render) {
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main() {
    SDL_Window *win = NULL;
    SDL_Renderer *render = NULL;
    init(win, render);

    std::array<std::array<bool, MAPWIDTH>, MAPHEIGHT> map = {};
    
    initRandomMap(map);
    
    // create specific patterns
    // initGosperGliderGun(map);
    // initPufferTrain(map);
    // initSimkinGliderGun(map);

    draw(render, map);

    int timeCounter = 0;
    bool isQuit = false;
    SDL_Event event;
    
    // Main loop
    while (!isQuit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isQuit = true;
            }
        }

        if (timeCounter >= DELAY) {
            timeCounter = 0;
            lifePass(map);
            draw(render, map);
        } else {
            timeCounter++;
        }
    }

    close(win, render);
    return 0;
}
