#include "game_logic.h"
#include <math.h>
#include <stdlib.h>

void InitGame(GameState* state) {
    state->leftPaddle.x = 30;
    state->leftPaddle.y = (float)SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    state->leftPaddle.score = 0;

    state->rightPaddle.x = SCREEN_WIDTH - 30 - PADDLE_WIDTH;
    state->rightPaddle.y = (float)SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    state->rightPaddle.score = 0;

    state->ball.x = (float)SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    state->ball.y = (float)SCREEN_HEIGHT / 2 - BALL_SIZE / 2;
    state->ball.vx = INITIAL_SPEED_X;
    state->ball.vy = INITIAL_SPEED_Y;
    state->ball.speedMultiplier = 1.0f;  

    state->gameRunning = 1;
    state->winningScore = 5;  

void ResetBall(GameState* state, int serveToLeft) {
    state->ball.x = (float)SCREEN_WIDTH / 2 - BALL_SIZE / 2;
    state->ball.y = (float)SCREEN_HEIGHT / 2 - BALL_SIZE / 2;

    state->ball.speedMultiplier = 1.0f;

    if (serveToLeft) {
        state->ball.vx = -INITIAL_SPEED_X;  
    }
    else {
        state->ball.vx = INITIAL_SPEED_X;   
    }

    state->ball.vy = (float)((rand() % 7) - 3);
    if (state->ball.vy > -1.0f && state->ball.vy < 1.0f) {
        state->ball.vy = (state->ball.vy > 0) ? 2.0f : -2.0f;
    }
}

void UpdateGame(GameState* state, int leftUp, int leftDown, int rightUp, int rightDown) {
    if (!state->gameRunning) return;

    float paddleSpeed = 8.0f;

    if (leftUp && state->leftPaddle.y > 0) {
        state->leftPaddle.y -= paddleSpeed;
    }
    if (leftDown && state->leftPaddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        state->leftPaddle.y += paddleSpeed;
    }

    if (rightUp && state->rightPaddle.y > 0) {
        state->rightPaddle.y -= paddleSpeed;
    }
    if (rightDown && state->rightPaddle.y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        state->rightPaddle.y += paddleSpeed;
    }

    state->ball.x += state->ball.vx * state->ball.speedMultiplier;
    state->ball.y += state->ball.vy * state->ball.speedMultiplier;

    if (state->ball.y <= 0) {
        state->ball.y = 0;
        state->ball.vy = -state->ball.vy;
        state->ball.speedMultiplier *= WALL_BOOST;
        if (state->ball.speedMultiplier > MAX_SPEED_MULTIPLIER) {
            state->ball.speedMultiplier = MAX_SPEED_MULTIPLIER;
        }
    }

    if (state->ball.y + BALL_SIZE >= SCREEN_HEIGHT) {
        state->ball.y = SCREEN_HEIGHT - BALL_SIZE;
        state->ball.vy = -state->ball.vy;
        state->ball.speedMultiplier *= WALL_BOOST;
        if (state->ball.speedMultiplier > MAX_SPEED_MULTIPLIER) {
            state->ball.speedMultiplier = MAX_SPEED_MULTIPLIER;
        }
    }

    if (state->ball.x <= state->leftPaddle.x + PADDLE_WIDTH &&
        state->ball.x + BALL_SIZE >= state->leftPaddle.x &&
        state->ball.y + BALL_SIZE >= state->leftPaddle.y &&
        state->ball.y <= state->leftPaddle.y + PADDLE_HEIGHT) {


        state->ball.vx = INITIAL_SPEED_X;

        float hitPos = (state->ball.y + BALL_SIZE / 2) - (state->leftPaddle.y + PADDLE_HEIGHT / 2);
        float maxAngle = 8.0f;
        state->ball.vy = (hitPos / (PADDLE_HEIGHT / 2)) * maxAngle;

        if (state->ball.vy > 8.0f) state->ball.vy = 8.0f;
        if (state->ball.vy < -8.0f) state->ball.vy = -8.0f;

        state->ball.speedMultiplier *= PADDLE_BOOST;
        if (state->ball.speedMultiplier > MAX_SPEED_MULTIPLIER) {
            state->ball.speedMultiplier = MAX_SPEED_MULTIPLIER;
        }

        if (rand() % 100 < 15) {  
            state->ball.vy += (float)((rand() % 3) - 1);
        }

        state->ball.x = state->leftPaddle.x + PADDLE_WIDTH;
    }

    if (state->ball.x + BALL_SIZE >= state->rightPaddle.x &&
        state->ball.x <= state->rightPaddle.x + PADDLE_WIDTH &&
        state->ball.y + BALL_SIZE >= state->rightPaddle.y &&
        state->ball.y <= state->rightPaddle.y + PADDLE_HEIGHT) {

        state->ball.vx = -INITIAL_SPEED_X;

        float hitPos = (state->ball.y + BALL_SIZE / 2) - (state->rightPaddle.y + PADDLE_HEIGHT / 2);
        float maxAngle = 8.0f;
        state->ball.vy = (hitPos / (PADDLE_HEIGHT / 2)) * maxAngle;

        if (state->ball.vy > 8.0f) state->ball.vy = 8.0f;
        if (state->ball.vy < -8.0f) state->ball.vy = -8.0f;

        state->ball.speedMultiplier *= PADDLE_BOOST;
        if (state->ball.speedMultiplier > MAX_SPEED_MULTIPLIER) {
            state->ball.speedMultiplier = MAX_SPEED_MULTIPLIER;
        }

        if (rand() % 100 < 15) {
            state->ball.vy += (float)((rand() % 3) - 1);
        }
        
        state->ball.x = state->rightPaddle.x - BALL_SIZE;
    }

    if (state->ball.x + BALL_SIZE < 0) {
        state->rightPaddle.score++;
        ResetBall(state, 0);  
    }

    if (state->ball.x > SCREEN_WIDTH) {
        state->leftPaddle.score++;
        ResetBall(state, 1);  
    }

    if (state->leftPaddle.score >= state->winningScore ||
        state->rightPaddle.score >= state->winningScore) {
        state->gameRunning = 0;
    }
}
