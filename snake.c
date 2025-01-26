#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL3/SDL.h>

#define WIDTH 901
#define HEIGHT 601
#define CELL_SIZE 15
#define GRID_SIZE 1
#define ROWS HEIGHT / CELL_SIZE
#define COLUMNS WIDTH / CELL_SIZE
#define SNAKE_COLOR 0x3df407
#define APPLE_COLOR 0xf42b07

void DrawGrid(SDL_Surface* surf)
{
    SDL_ClearSurface(surf, 0, 0, 0, 1);
    // Draw columns
    for (int x = 0; x < WIDTH; x += CELL_SIZE)
    {
        for (int y = 0; y < HEIGHT; ++y)
        {
            SDL_Rect rect = {x, y, GRID_SIZE, GRID_SIZE};
            SDL_FillSurfaceRect(surf, &rect, 0x717171);
        }
    }

    // Draw rows
    for (int y = 0; y < HEIGHT; y += CELL_SIZE)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            SDL_Rect rect = {x, y, GRID_SIZE, GRID_SIZE};
            SDL_FillSurfaceRect(surf, &rect, 0x717171);
        }
    }
}

// Draw the rect in the grid with given position

void DrawRect(SDL_Surface* surface, int x, int y, int color)
{
    SDL_Rect rect = {x * CELL_SIZE + GRID_SIZE, y * CELL_SIZE + GRID_SIZE, CELL_SIZE - GRID_SIZE, CELL_SIZE - GRID_SIZE};
    SDL_FillSurfaceRect(surface, &rect, color);
}

typedef struct Body
{
    int x, y;
} Body;

typedef struct Apple
{
    int x, y;
} Apple;

float snakeMoveCounter = 0.0f;
float snakeMovePeriod = 0.24f;
int vel_x = 1, vel_y = 0;
int isGameOver = false;
int windowClosed = 0;
Body* snake;
Apple apple;
int score = 1;

void DrawSurface(SDL_Surface* surface)
{
    DrawGrid(surface);
    for (int i = 0; i < score; ++i)
    {
        DrawRect(surface, snake[i].x, snake[i].y, SNAKE_COLOR);
    }
    DrawRect(surface, apple.x, apple.y, APPLE_COLOR);
}

bool CheckAppleCollision(Body body, Apple apple)
{
    return body.x == apple.x && body.y == apple.y;
}

bool SpawnAppleHelper(Apple newApple)
{
    bool coll = false;
    for (int i = 0; i < score; ++i)
    {
        if (CheckAppleCollision(snake[i], newApple) || (apple.x == newApple.x && apple.y == newApple.y)) coll = true;
    }
    return coll;
}

void SpawnApple()
{
    int rng = clock();
    Apple newApple = {rng % COLUMNS, rng % ROWS};
    int count = 0;
    
    while(SpawnAppleHelper(newApple))
    {
        rng = clock() + count;
        newApple = (Apple){rng % COLUMNS, rng % ROWS};
        count++;
    }
    apple = newApple;
}

void IncrementScore()
{   
    snake[score] = snake[score - 1];
    ++score;
}

bool CheckBodyCollision()
{
    for (int i = 1; i < score; ++i)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) return true;
    }
    return false;
}

float max(float x, float y)
{
    return (x > y ? x : y);
}

void Update(float dt)
{

    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            
            switch (event.key.key)
            {
            case SDLK_UP:
                if (vel_y != 1)
                {
                    vel_x = 0;
                    vel_y = -1;
                }
                break;
            case SDLK_DOWN:
                if (vel_y != -1)
                {
                    vel_x = 0;
                    vel_y = 1;
                }
                break;
            case SDLK_LEFT:
                if (vel_x != 1)
                {
                    vel_x = -1;
                    vel_y = 0;
                }
                break;
            case SDLK_RIGHT:
                if (vel_x != -1)
                {
                    vel_x = 1;
                    vel_y = 0;
                }
            }
            break;
        case SDL_EVENT_QUIT:
            windowClosed = 1;                
            break;
        }
    }

    if (!isGameOver)
    {
        snakeMoveCounter += dt;

        if (snakeMoveCounter >= snakeMovePeriod)
        {
            snakeMoveCounter = 0.0f;

            for (int i = score - 1; i >= 1; --i)
            {
                snake[i] = snake[i - 1]; 
            }

            snake[0].x += vel_x;
            snake[0].y += vel_y;
        
            if (snake[0].y < 0) snake[0].y = ROWS - 1;
            if (snake[0].y > ROWS) snake[0].y = 0;
            if (snake[0].x < 0) snake[0].x = COLUMNS - 1;
            if (snake[0].x > COLUMNS) snake[0].x = 0;

            isGameOver = CheckBodyCollision();

            if (snake[0].x == apple.x && snake[0].y == apple.y) 
            {
                SpawnApple();
                IncrementScore();
                snakeMovePeriod = max(snakeMovePeriod - 0.01f, 0.08f);
            }
        }
    }
}

float prevTime = 0.0f;

float GetDeltaTime()
{
    float old = prevTime;
    prevTime = clock();
    return ((float)(prevTime - old)) / CLOCKS_PER_SEC;
}

int main()
{
    SDL_Init( SDL_INIT_VIDEO );
    SDL_Window* window = SDL_CreateWindow("Snake Game", WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    prevTime = clock();
    snake = malloc(sizeof(Body) * ROWS * COLUMNS);
    snake[0] = (Body){3, 3};
    apple = (Apple){0, 0};

    while(!windowClosed)
    {
        float dt = GetDeltaTime();
        Update(dt);
        DrawSurface(surface);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
