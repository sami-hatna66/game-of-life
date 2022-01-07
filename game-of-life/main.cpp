//
//  main.cpp
//  game-of-life
//
//  Created by Sami Hatna on 06/01/2022.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <array>
#include <random>
using namespace std;

void init();
void draw();
void close();
void initScreenMap();
void lifePass();
void initGosperGliderGun();
void initSimkinGliderGun();
void initPufferTrain();
void initEmptyMap();

SDL_Window* win = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* render = NULL;

// CONFIG
const int mapWidth = 150; // num of cells along x
const int mapHeight = 102; // num of cells along y
const int scale = 5; // scale of each cell
const int delay = 25000; // time between each lifecycle

// cell object
class cell {
private:
    bool status;
public:
    // rect is public because renderer requires address of rect which can't be acquired through a temp return value
    SDL_Rect rect;
    cell(SDL_Rect r, bool s) {
        rect = r;
        status = s;
    }
    cell() {
        status = false;
    }
    void setStatus(bool newStatus) {
        status = newStatus;
    }
    bool getStatus() {
        return status;
    }
};

// array of cell objects
array<array<cell, mapWidth>, mapHeight> screenMap = {};

// init SDL window
void init() {
    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("The Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mapWidth * scale, mapHeight * scale, SDL_WINDOW_SHOWN);
}

// fill window with random living cells
void initScreenMap() {
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            SDL_Rect r;
            r.x = j * scale; r.y = i * scale; r.w = scale; r.h = scale;
            screenMap[i][j].rect = r;
            
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> distr(0, 100);
            int result = distr(gen);
            screenMap[i][j].setStatus(result >= 25 ? false : true);
        }
    }
}

// create an map with no living cells
// helper for Simkin, Gosper and Puffer functions
void initEmptyMap() {
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            SDL_Rect r;
            r.x = j * scale; r.y = i * scale; r.w = scale; r.h = scale;
            screenMap[i][j].rect = r;
            
            screenMap[i][j].setStatus(false);
        }
    }
}

// create empty map and populate with Simkin's glider gun pattern (2015)
void initSimkinGliderGun() {
    initEmptyMap();
    
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
        screenMap[gunCoords[k][0]][gunCoords[k][1]].setStatus(true);
    }
}

// create empty map and populate with Gosper's glider gun pattern (1970)
void initGosperGliderGun() {
    initEmptyMap();
    
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
        screenMap[gunCoords[k][0]][gunCoords[k][1]].setStatus(true);
    }
}

// create empty map and populate with Gosper's Puffer 1 pattern (1971)
void initPufferTrain() {
    initEmptyMap();
    
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
        screenMap[pufferCoords[k][0]][pufferCoords[k][1]].setStatus(true);
    }
}

// one pass in the cellular lifecycle
void lifePass() {
    array<array<cell, mapWidth>, mapHeight> copy = screenMap;
    
    for (int i = 0; i < mapHeight; i++) { // row
        for (int j = 0; j < mapWidth; j++) { // column
            int liveNeighbourCount = 0;
            
            bool left = (j == 0) ? copy[i][mapWidth - 1].getStatus() : copy[i][j - 1].getStatus();
            
            bool right = (j == mapWidth - 1) ? copy[i][0].getStatus() : copy[i][j + 1].getStatus();
            
            bool above = (i == 0) ? copy[mapHeight - 1][j].getStatus() : copy[i - 1][j].getStatus();
            
            bool below = (i == mapHeight - 1) ? copy[0][j].getStatus() : copy[i + 1][j].getStatus();
            
            bool upleftdiag = false;
            if (!(i == 0 & j == 0)) {
                upleftdiag = (j == 0) ? copy[i - 1][mapWidth - 1].getStatus() : ((i == 0) ?  copy[mapHeight - 1][j - 1].getStatus() : copy[i - 1][j - 1].getStatus());
            }
                
            bool uprightdiag = false;
            if (!(i == 0 & j == mapWidth - 1)) {
                uprightdiag = (j == mapWidth - 1) ? copy[i - 1][0].getStatus() : ((i == 0) ? copy[mapHeight - 1][j + 1].getStatus() : copy[i - 1][j + 1].getStatus());
            }
                
            bool downleftdiag = false;
            if (!(i == mapHeight - 1 & j == 0)) {
                downleftdiag = (j == 0) ? copy[i + 1][mapWidth - 1].getStatus() : ((i == mapHeight - 1) ? copy[0][j - 1].getStatus() : copy[i + 1][j - 1].getStatus());
            }
            
            bool downrightdiag = false;
            if (!(i == mapHeight - 1 &  j == mapWidth - 1)) {
                downrightdiag = (j == mapWidth - 1) ? copy[i + 1][0].getStatus() : ((i == mapHeight - 1) ? copy[0][j + 1].getStatus() : copy[i + 1][j + 1].getStatus());
            }
                
            bool boolList[8] = {above, below, left, right, upleftdiag, uprightdiag, downleftdiag, downrightdiag};
            
            for (int i = 0; i < sizeof(boolList); i++) {
                if (boolList[i]) {
                    liveNeighbourCount++;
                }
            }
            
            if (screenMap[i][j].getStatus()) {
                if (liveNeighbourCount < 2) {
                    screenMap[i][j].setStatus(false);
                }
                else if (liveNeighbourCount > 3) {
                    screenMap[i][j].setStatus(false);
                }
            }
            else {
                if (liveNeighbourCount == 3) {
                    screenMap[i][j].setStatus(true);
                }
            }
        }
    }
}

// draw frame
void draw() {
    SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
    SDL_RenderClear(render);
    
    for (int i = 0; i < mapHeight; i++) {
        for (int j = 0; j < mapWidth; j++) {
            if (screenMap[i][j].getStatus()) {
                SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
                SDL_RenderFillRect(render, &screenMap[i][j].rect);
            }
        }
    }
    
    SDL_RenderPresent(render);
}

void close() {
    SDL_DestroyWindow(win);
    SDL_Quit();
}

int main() {
    init();
    render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    // create random board of cells
    initScreenMap();

    // create specific patterns
    //initGosperGliderGun();
    //initPufferTrain();
    //initSimkinGliderGun();

    draw();
    
    // Main Loop
    int count = 0;
    bool isquit = false;
    SDL_Event event;
    while (!isquit) {
        if (SDL_PollEvent( & event)) {
            if (event.type == SDL_QUIT) {
                isquit = true;
            }
        }
        
        if (count >= delay) {
            count = 0;
            lifePass();
            draw();
        }
        else {
            count++;
        }
    }
    
    close();
    
    return 0;
}
