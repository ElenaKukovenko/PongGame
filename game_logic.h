#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  600

#define PADDLE_WIDTH   20
#define PADDLE_HEIGHT  100

#define BALL_SIZE      20

#define INITIAL_SPEED_X    5.0f
#define INITIAL_SPEED_Y    3.0f
#define MAX_SPEED_MULTIPLIER 3.0f
#define WALL_BOOST         1.05f   
#define PADDLE_BOOST       1.10f   

typedef struct {
    float x, y;
    int score;
} Paddle;

typedef struct {
    float x, y;
    float vx, vy;
    float speedMultiplier;
} Ball;

typedef struct {
    Paddle leftPaddle;
    Paddle rightPaddle;
    Ball ball;
    int gameRunning;
    int winningScore;
} GameState;

void InitGame(GameState* state);

void UpdateGame(GameState* state, int leftUp, int leftDown, int rightUp, int rightDown);

void ResetBall(GameState* state, int serveToLeft);