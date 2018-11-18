#pragma once

#include "SDL2/SDL.h"
#include <vector>


struct Vector2 {
    float x;
    float y;
};

struct Ball {
    Vector2 pos;
    Vector2 vel;
};

struct Game {
    Game();
    bool Initialize();
    void RunLoop();
    void Shutdown();
private:
    Uint16 thickness;
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    bool mIsRunning;
    Uint32 mTicksCount;
    std::vector<Ball> balls;
    Vector2 mPaddlePos;
    Vector2 mPaddle2Pos;
    int mPaddleDir;
    int mPaddle2Dir;
};

