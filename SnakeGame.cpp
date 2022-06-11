#include <iostream>
#include <curses.h>
#include <panel.h>
#include <Windows.h>
#include <string>

using namespace std;

int timeUnit = 50;  //�Է� ����
int ticUnit = 500;  //1tic�� ms

int snakeBoardX = 43;   //board ���α���
int snakeBoardY = 19;   //board ���α���
int maxSnakeLength = snakeBoardX * snakeBoardY; //snake �ִ����

int** snakeBoard, ** snake;
int snakeLength = 0;    //���� snake ����

//Ű �Է� Ȯ��
int keyState(int key) {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)   // esc: ������
        return -100;

    int newkey = key;
    if (GetAsyncKeyState(VK_UP) & 0x8000)  // 1 : ��
        newkey = 1;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)   // 2 : ��
        newkey = 2;
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000)    // 3 : ��
        newkey = 3;
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000)    // 4 : ��
        newkey = 4;

    if ((newkey == 1 && key == 3) || (newkey == 3 && key == 1) ||   //-1 : �ݴ�����Է�
        (newkey == 2 && key == 4) || (newkey == 4 && key == 2))
        return -1;
    else return newkey;
}

//���� �̵� ����(����)
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

//���� �̵� ����(����)
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

//snake �׸���
void drawSnake() {
    for (int i = 1; i < snakeLength; i++)
        snakeBoard[snake[i][0]][snake[i][1]] = 1;
    snakeBoard[snake[0][0]][snake[0][1]] = 2;
}

//board ����
//board ��Ģ : 
// -1 : �� (���� �Ұ���)
//  0 : �� ����(���� ����)
//  1 : snake ����
//  2 : snake �Ӹ�
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

//snake �̵�
// status = 0 or etc : �⺻ �̵�
// status = 1 : ���� ���� �� �̵�
// status = -1 : ���� ���� �� �̵�
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
    if (status == 1) { //���� ����
        snake[snakeLength] = new int[2];
        snake[snakeLength][0] = tempY;
        snake[snakeLength][1] = tempX;
        snake[snakeLength + 1] = NULL;
        snakeLength++;
    }
    else if (status == -1 && snakeLength > 1) { //���� ����
        snake[snakeLength - 1] = NULL;
        snakeLength--;
    }

    drawSnake();
}

//�ʱ� snake ��ġ �� ���� ����
void initSnake() {
    snake = new int* [maxSnakeLength];
    snakeLength = 1;

    snake[0] = new int[2];
    snake[0][0] = snakeBoardY / 2 - 1;
    snake[0][1] = snakeBoardX / 2 - 2;

    moveSnake(2, 1);
    moveSnake(2, 1);
}

//�ش� ��ġ�� ������ �߰�
//type 1 : Growth
//type -1 : Poison
void addItem(int y, int x, int type) {
    if (type == 1)
        snakeBoard[y][x] = 3;
    else if (type == -1)
        snakeBoard[y][x] = 4;
}

//���� ��ġ(���� ĭ)�� item Ȯ��
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

//���� ĭ ����� �浹 ���� Ȯ��(���� or �� �浹)
bool isConflict(int key) {
    int y = snake[0][0] + nextY(key);
    int x = snake[0][1] + nextX(key);
    return snakeBoard[y][x] == 1 || snakeBoard[y][x] == -1;
}

//window�� board ���
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
        if (key < 0) break; //�ݴ���� �Է� or esc �Է½� ����
        if (isConflict(key)) break;  //�� or ���� �浹�� ����

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

