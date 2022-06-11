#include <iostream>
#include <curses.h>
#include <panel.h>
#include <Windows.h>
#include <string>

using namespace std;

int timeUnit = 50;  //입력 단위
int ticUnit = 500;  //1tic당 ms

int snakeBoardX = 43;   //board 가로길이
int snakeBoardY = 19;   //board 세로길이
int maxSnakeLength = snakeBoardX * snakeBoardY; //snake 최대길이

int** snakeBoard, ** snake;
int snakeLength = 0;    //현재 snake 길이

//키 입력 확인
int keyState(int key) {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)   // esc: 끝내기
        return -100;

    int newkey = key;
    if (GetAsyncKeyState(VK_UP) & 0x8000)  // 1 : 상
        newkey = 1;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)   // 2 : 우
        newkey = 2;
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)    // 3 : 하
        newkey = 3;
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000)    // 4 : 좌
        newkey = 4;

    if ((newkey == 1 && key == 3) || (newkey == 3 && key == 1) ||   //-1 : 반대방향입력
        (newkey == 2 && key == 4) || (newkey == 4 && key == 2))
        return -1;
    else return newkey;
}

//다음 이동 방향(가로)
int nextX(int key) {
    int x;
    if (key == 2)
        x = 1;
    else if (key == 4)
        x = -1;
    else
        x = 0;
    return x;
}

//다음 이동 방향(세로)
int nextY(int key) {
    int y;
    if (key == 1)
        y = -1;
    else if (key == 3)
        y = 1;
    else
        y = 0;
    return y;
}

//snake 그리기
void drawSnake() {
    for (int i = 1; i < snakeLength; i++)
        snakeBoard[snake[i][0]][snake[i][1]] = 1;
    snakeBoard[snake[0][0]][snake[0][1]] = 2;
}

//board 생성
//board 규칙 : 
// -1 : 벽 (진행 불가능)
//  0 : 빈 공간(진행 가능)
//  1 : snake 꼬리
//  2 : snake 머리
//  3 : Item - Growth
//  4 : Item - Poison
//  5 : Item - Gate
void initSnakeBoard() {
    snakeBoard = new int* [snakeBoardY];
    for (int i = 0; i < snakeBoardY; i++) {
        snakeBoard[i] = new int[snakeBoardX];
        for (int j = 0; j < snakeBoardX; j++) {
            if (i == 0 || j == 0 || i == snakeBoardY - 1 || j == snakeBoardX - 1)
                snakeBoard[i][j] = -1;
            else
                snakeBoard[i][j] = 0;
        }
    }
}

//snake 이동
// status = 0 or etc : 기본 이동
// status = 1 : 길이 증가 및 이동
// status = -1 : 길이 감소 및 이동
void moveSnake(int key, int status) {
    int tempY = snake[0][0];
    int tempX = snake[0][1];

    snakeBoard[tempY][tempX] = 0;
    snake[0][0] += nextY(key);
    snake[0][1] += nextX(key);

    int temp2X, temp2Y;
    for (int i = 1; i < snakeLength; i++) {
        snakeBoard[snake[i][0]][snake[i][1]] = 0;
        temp2Y = snake[i][0];
        temp2X = snake[i][1];
        snake[i][0] = tempY;
        snake[i][1] = tempX;
        tempY = temp2Y;
        tempX = temp2X;
    }
    if (status == 1) { //길이 증가
        snake[snakeLength] = new int[2];
        snake[snakeLength][0] = tempY;
        snake[snakeLength][1] = tempX;
        snake[snakeLength + 1] = NULL;
        snakeLength++;
    }
    else if (status == -1 && snakeLength > 1) { //길이 감소
        snake[snakeLength - 1] = NULL;
        snakeLength--;
    }

    drawSnake();
}

//초기 snake 위치 및 길이 설정
void initSnake() {
    snake = new int* [maxSnakeLength];
    snakeLength = 1;

    snake[0] = new int[2];
    snake[0][0] = snakeBoardY / 2 - 1;
    snake[0][1] = snakeBoardX / 2 - 2;

    moveSnake(2, 1);
    moveSnake(2, 1);
}

//해당 위치에 아이템 추가
//type 1 : Growth
//type -1 : Poison
void addItem(int y, int x, int type) {
    if (type == 1)
        snakeBoard[y][x] = 3;
    else if (type == -1)
        snakeBoard[y][x] = 4;
}

//진행 위치(다음 칸)의 item 확인
//type 1 : Growth
//type -1 : Poison
int checkItem(int key) {
    int y = snake[0][0] + nextY(key);
    int x = snake[0][1] + nextX(key);
    if (snakeBoard[y][x] == 3)
        return 1;
    else if (snakeBoard[y][x] == 4)
        return -1;
    else
        return 0;
}

//다음 칸 진행시 충돌 여부 확인(꼬리 or 벽 충돌)
bool isConflict(int key) {
    int y = snake[0][0] + nextY(key);
    int x = snake[0][1] + nextX(key);
    return snakeBoard[y][x] == 1 || snakeBoard[y][x] == -1;
}

//window에 board 출력
void printSnakeBoard(WINDOW* win) {
    string line;
    for (int i = 0; i < snakeBoardY; i++) {
        line = "";
        for (int j = 0; j < snakeBoardX; j++) {
            if (snakeBoard[i][j] == -1)
                line += "&";
            else if (snakeBoard[i][j] == 0)
                line += " ";
            else if (snakeBoard[i][j] == 1)
                line += "O";
            else if (snakeBoard[i][j] == 2)
                line += "H";
            else if (snakeBoard[i][j] == 3)
                line += "G";
            else if (snakeBoard[i][j] == 4)
                line += "P";
            else if (snakeBoard[i][j] == 5)
                line += "@";
        }
        mvwprintw(win, i + 1, 2, line.c_str());
    }

    wrefresh(win);
}

int main()
{
    WINDOW* snakeWin;
    initscr();
    resize_term(25, 52);
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    border('*', '*', '*', '*', '*', '*', '*', '*');
    mvprintw(1, 1, "Snake Game");
    refresh();


    snakeWin = newwin(21, 47, 3, 2);
    wbkgd(snakeWin, COLOR_PAIR(1));
    wattron(snakeWin, COLOR_PAIR(1));
    mvwprintw(snakeWin, snakeBoardY / 2 - 1, 2, "Press space to start...");
    wrefresh(snakeWin);
    getchar();
    mvwprintw(snakeWin, snakeBoardY / 2 - 1, 2, "                         ");

    int i = 0;
    int tic = 0;
    int key = 2;
    initSnakeBoard();
    initSnake();

    addItem(3, 10, 1);
    addItem(5, 10, 1);
    addItem(7, 10, 1);
    addItem(9, 10, -1);

    while (1) {
        Sleep(timeUnit);
        key = keyState(key);
        if (key < 0) break; //반대방향 입력 or esc 입력시 종료
        if (isConflict(key)) break;  //벽 or 꼬리 충돌시 종료

        if (tic >= ticUnit) {
            moveSnake(key, checkItem(key));
            printSnakeBoard(snakeWin);
            tic = 0;
        }

        i++;
        tic += timeUnit;
    }

    mvwprintw(snakeWin, snakeBoardY / 2 - 1, 2, "Fail...");
    wrefresh(snakeWin);
    getchar();

    delwin(snakeWin);
    endwin();


    return 0;
}

