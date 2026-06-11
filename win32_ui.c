#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "gdi32.lib")

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"

static GameState gameState;
static HWND mainHwnd;

static HBRUSH hBlackBrush = NULL;
static HBRUSH hWhiteBrush = NULL;


void Render(HDC hdc, int width, int height) {
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP hOldBitmap = SelectObject(hdcBuffer, hBitmap);

    RECT rect = { 0, 0, width, height };
    FillRect(hdcBuffer, &rect, hBlackBrush);

    HPEN pen = CreatePen(PS_DOT, 2, RGB(255, 255, 255));
    HPEN oldPen = SelectObject(hdcBuffer, pen);
    MoveToEx(hdcBuffer, width / 2, 0, NULL);
    LineTo(hdcBuffer, width / 2, height);
    SelectObject(hdcBuffer, oldPen);
    DeleteObject(pen);

    pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    SelectObject(hdcBuffer, pen);
    Ellipse(hdcBuffer, width / 2 - 50, height / 2 - 50, width / 2 + 50, height / 2 + 50);
    DeleteObject(pen);

    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdcBuffer, brush);
    Rectangle(hdcBuffer, (int)gameState.leftPaddle.x, (int)gameState.leftPaddle.y,
        (int)gameState.leftPaddle.x + PADDLE_WIDTH,
        (int)gameState.leftPaddle.y + PADDLE_HEIGHT);
    DeleteObject(brush);    

    brush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdcBuffer, brush);
    Rectangle(hdcBuffer, (int)gameState.rightPaddle.x, (int)gameState.rightPaddle.y,
        (int)gameState.rightPaddle.x + PADDLE_WIDTH,
        (int)gameState.rightPaddle.y + PADDLE_HEIGHT);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(255, 100, 100));
    SelectObject(hdcBuffer, brush);
    Ellipse(hdcBuffer, (int)gameState.ball.x, (int)gameState.ball.y,
        (int)gameState.ball.x + BALL_SIZE, (int)gameState.ball.y + BALL_SIZE);
    DeleteObject(brush);

    char scoreText[100];
    sprintf_s(scoreText, sizeof(scoreText), "LEFT: %d          RIGHT: %d",
        gameState.leftPaddle.score, gameState.rightPaddle.score);

    SetTextColor(hdcBuffer, RGB(255, 255, 0));
    SetBkMode(hdcBuffer, TRANSPARENT);

    HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Arial");
    HFONT oldFont = SelectObject(hdcBuffer, hFont);

    TextOut(hdcBuffer, width / 2 - 150, 20, scoreText, (int)strlen(scoreText));

    char speedText[50];
    sprintf_s(speedText, sizeof(speedText), "SPEED: %.1fx", gameState.ball.speedMultiplier);
    SetTextColor(hdcBuffer, RGB(200, 200, 100));
    HFONT hSmallFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Arial");
    SelectObject(hdcBuffer, hSmallFont);
    TextOut(hdcBuffer, width - 120, 20, speedText, (int)strlen(speedText));
    DeleteObject(hSmallFont);

    if (!gameState.gameRunning) {
        char winnerText[100];
        if (gameState.leftPaddle.score >= gameState.winningScore) {
            sprintf_s(winnerText, sizeof(winnerText), "LEFT PLAYER WINS! Press ESC to exit");
        }
        else {
            sprintf_s(winnerText, sizeof(winnerText), "RIGHT PLAYER WINS! Press ESC to exit");
        }
        SetTextColor(hdcBuffer, RGB(255, 100, 100));
        hFont = CreateFont(36, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH, "Arial");
        SelectObject(hdcBuffer, hFont);
        TextOut(hdcBuffer, width / 2 - 200, height / 2 - 50, winnerText, (int)strlen(winnerText));
        DeleteObject(hFont);
    }

    SelectObject(hdcBuffer, oldFont);
    DeleteObject(hFont);

    char controlsText[] = "W/S  |  UP/DOWN  |  ESC to exit";
    SetTextColor(hdcBuffer, RGB(150, 150, 150));
    hSmallFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Arial");
    SelectObject(hdcBuffer, hSmallFont);
    TextOut(hdcBuffer, width / 2 - 100, height - 30, controlsText, (int)strlen(controlsText));
    DeleteObject(hSmallFont);

    BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);
    SelectObject(hdcBuffer, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBuffer);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        Render(hdc, rect.right, rect.bottom);
        EndPaint(hwnd, &ps);
        return 0;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int RunGame(HINSTANCE hInstance, int nCmdShow) {
    srand((unsigned int)time(NULL));

    const char CLASS_NAME[] = "PongGameWindow";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Failed to register window class", "Error", MB_OK);
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "PONG - Tennis for Two Players | Speed increases with each hit!",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        SCREEN_WIDTH + 16, SCREEN_HEIGHT + 39,
        NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, "Failed to create window", "Error", MB_OK);
        return 1;
    }

    mainHwnd = hwnd;

    hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
    hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    InitGame(&gameState);

    int leftUp = 0, leftDown = 0;
    int rightUp = 0, rightDown = 0;
    MSG msg = { 0 };

    while (gameState.gameRunning || msg.message != WM_QUIT) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        leftUp = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
        leftDown = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
        rightUp = (GetAsyncKeyState(VK_UP) & 0x8000) ? 1 : 0;
        rightDown = (GetAsyncKeyState(VK_DOWN) & 0x8000) ? 1 : 0;

        UpdateGame(&gameState, leftUp, leftDown, rightUp, rightDown);

        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
            
        Sleep(16);
    }

    DeleteObject(hBlackBrush);
    DeleteObject(hWhiteBrush);

    return 0;
}
