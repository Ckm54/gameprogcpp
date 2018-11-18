#include "game.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PADDLE_HEIGHT 100

bool rectIntersects(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return 
        x1 - w1 / 2.0f < x2 + w2 / 2.0f &&
        x1 + w1 / 2.0f > x2 - w2 / 2.0f &&
        y1 - h1 / 2.0f < y2 + h2 / 2.0f &&
        y1 + h1 / 2.0f > y2 - h2 / 2.0f;
}
        
Game::Game():
    mTicksCount(0),
    mIsRunning(false),
    thickness(15),
    mPaddleDir(0),
    mPaddle2Dir(0)
{
    mPaddlePos.x = thickness;
    mPaddlePos.y = WINDOW_HEIGHT / 2.0f;

    mPaddle2Pos.x = WINDOW_WIDTH - thickness;
    mPaddle2Pos.y = WINDOW_HEIGHT / 2.0f;
}

bool Game::Initialize() {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    mWindow = SDL_CreateWindow("Memes lol", 100, 1600, 1024, 768, 0);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }
    mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!mRenderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    // initialize the balls!
    Ball ball;
    ball.vel.x = 10.0f;
    ball.vel.y = 10.0f;
    ball.pos.x = 100.0f;
    ball.pos.y = 100.0f;

    balls.push_back(ball);

    ball.vel.x = 50.0f;
    ball.vel.y = 50.0f;
    ball.pos.x = 350.0f;
    ball.pos.y = 470.0f;

    balls.push_back(ball);
    return true;
}

void Game::RunLoop() {
    mIsRunning = true;
    while (mIsRunning) {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::ProcessInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }
    }
    
    const Uint8* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }

    // update paddle direction
    mPaddleDir = 0;
    mPaddle2Dir = 0;

    if(state[SDL_SCANCODE_W]) {
        mPaddleDir -= 1;
    }
    if(state[SDL_SCANCODE_S]) {
        mPaddleDir += 1;
    }

    if(state[SDL_SCANCODE_I]) {
        mPaddle2Dir -= 1;
    }
    if(state[SDL_SCANCODE_K]) {
        mPaddle2Dir += 1;
    }
}

void Game::UpdateGame() {
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
    // calculate time since last frame in seconds
    Uint32 ticks = SDL_GetTicks();
    float deltaTime = (ticks - mTicksCount) / 1000.0f;
    mTicksCount = ticks;
    if (deltaTime > 0.05f) deltaTime = 0.05f;

    // update paddle position
    mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
    if (mPaddlePos.y < PADDLE_HEIGHT/2.0f + thickness) {
        mPaddlePos.y = PADDLE_HEIGHT/2.0f + thickness;
    }
    if (mPaddlePos.y > WINDOW_HEIGHT - thickness - PADDLE_HEIGHT/2.0f) {
        mPaddlePos.y = WINDOW_HEIGHT - thickness - PADDLE_HEIGHT/2.0f;
    }

    mPaddle2Pos.y += mPaddle2Dir * 300.0f * deltaTime;
    if (mPaddle2Pos.y < PADDLE_HEIGHT/2.0f + thickness) {
        mPaddle2Pos.y = PADDLE_HEIGHT/2.0f + thickness;
    }
    if (mPaddle2Pos.y > WINDOW_HEIGHT - thickness - PADDLE_HEIGHT/2.0f) {
        mPaddle2Pos.y = WINDOW_HEIGHT - thickness - PADDLE_HEIGHT/2.0f;
    }

    for(auto & value: balls) {
        value.pos.x += value.vel.x * deltaTime;
        value.pos.y += value.vel.y * deltaTime;

        if (value.pos.y + thickness / 2.0f > WINDOW_HEIGHT - thickness) {
            value.vel.y = -value.vel.y;
            float offset = value.pos.y + thickness / 2.0f - (WINDOW_HEIGHT - thickness);
            value.pos.y -= offset * 2.0f;
        }

        if (value.pos.y - thickness / 2.0f < thickness) {
            value.vel.y = -value.vel.y;
            float offset = value.pos.y - thickness / 2.0f - thickness;
            value.pos.y -= offset * 2.0f;
        }

        // handle collision with paddle
        bool collides = rectIntersects(
                mPaddlePos.x, mPaddlePos.y, thickness, PADDLE_HEIGHT,
                value.pos.x,   value.pos.y,   thickness, thickness);

        if (collides && value.vel.x < 0) {
            value.vel.x = -value.vel.x;
        }

        collides = rectIntersects(
                mPaddle2Pos.x, mPaddle2Pos.y, thickness, PADDLE_HEIGHT,
                value.pos.x,   value.pos.y,   thickness, thickness);

        if (collides && value.vel.x > 0) {
            value.vel.x = -value.vel.x;
        }
    }
}

void Game::GenerateOutput() {
    SDL_SetRenderDrawColor(mRenderer, 0x95, 0x75, 0xcd, 0xff);
    SDL_RenderClear(mRenderer);

    // Rendering the walls yay
    SDL_SetRenderDrawColor(mRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_Rect wall {0, 0, 1024, thickness};
    SDL_RenderFillRect(mRenderer, &wall);
    wall.y = 768 - thickness;
    SDL_RenderFillRect(mRenderer, &wall);

    // Rendering the ball and paddle
    for (auto & value: balls) {
        SDL_Rect ball{
            static_cast<int>(value.pos.x - thickness/2.0f),
            static_cast<int>(value.pos.y - thickness/2.0f),
            thickness,
            thickness
        };
        SDL_RenderFillRect(mRenderer, &ball);
    }

    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x - thickness/2.0f),
        static_cast<int>(mPaddlePos.y - PADDLE_HEIGHT/2.0f),
        thickness,
        PADDLE_HEIGHT
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    SDL_Rect paddle2 {
        static_cast<int>(mPaddle2Pos.x - thickness/2.0f),
        static_cast<int>(mPaddle2Pos.y - PADDLE_HEIGHT/2.0f),
        thickness,
        PADDLE_HEIGHT
    };
    SDL_RenderFillRect(mRenderer, &paddle2);

    SDL_RenderPresent(mRenderer);
}
