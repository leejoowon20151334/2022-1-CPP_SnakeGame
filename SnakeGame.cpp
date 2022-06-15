#include <iostream>
#include <curses.h>
#include <panel.h>
#include <Windows.h>
#include <string>

using namespace std;

int timeUnit = 50;  //�Է� ����
int ticUnit = 500;  //1tic�� ms
int key; //key ������ġ ����

int snakeBoardX = 43;   //board ���α���
int snakeBoardY = 19;   //board ���α���
int maxSnakeLength = snakeBoardX * snakeBoardY; //snake �ִ����

int** snakeBoard, ** snake;
int snakeLength = 0;    //���� snake ����
int gate1X, gate1Y, gate2X, gate2Y; // 2���� Gate ��ǥ���� �߰�

// setKey �߰�
void setKey(int setKey) {
    key = setKey;
}

//���� ���� ��� (0���� number������ ���� ���)
int getRandomNumber(int number) {
    srand((unsigned int)time(NULL));
    int num = rand() % number;
    return num;
}

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
// -2 : Gate ���� �� (Immune Wall)
// -1 : �� (���� �Ұ���)
//  0 : �� ����(���� ����)
//  1 : snake ����
//  2 : snake �Ӹ�
//  3 : Item - Growth
//  4 : Item - Poison
//  5, 6 : Item - Gate
void initSnakeBoard() {
    snakeBoard = new int* [snakeBoardY];
    for (int i = 0; i < snakeBoardY; i++) {
        snakeBoard[i] = new int[snakeBoardX];
        for (int j = 0; j < snakeBoardX; j++) {
            if ((i == 0 || i == snakeBoardY - 1) && (j == 0 || j == snakeBoardX - 1))
                snakeBoard[i][j] = -2;
            else if (i == 0 || j == 0 || i == snakeBoardY - 1 || j == snakeBoardX - 1) {
                snakeBoard[i][j] = -1;
            }
            else
                snakeBoard[i][j] = 0;
        }
    }
}

//ImmuneWall ���� �޼��� �߰�(���� �����غ��� ����)
void setImmuneWall() {
    for (int i = 0; i < snakeBoardY; i++) {
        for (int j = 0; j < snakeBoardX; j++) {
            if (i == 0) {
                if (snakeBoard[i][j] <= -1 && snakeBoard[i + 1][j] <= -1)
                    snakeBoard[i][j] = -2;
            }
            else if (j == 0) {
                if (snakeBoard[i][j] <= -1 && snakeBoard[i][j + 1] <= -1)
                    snakeBoard[i][j] = -2;
            }
            else if (i == snakeBoardY - 1) {
                if (snakeBoard[i][j] <= -1 && snakeBoard[i - 1][j] <= -1)
                    snakeBoard[i][j] = -2;
            }
            else if (j == snakeBoardX - 1) {
                if (snakeBoard[i][j] <= -1 && snakeBoard[i][j - 1] <= -1)
                    snakeBoard[i][j] = -2;
            }
            else if (snakeBoard[i][j] <= -1 && snakeBoard[i - 1][j] <= -1 && snakeBoard[i + 1][j] <= -1 && snakeBoard[i][j - 1] <= -1 && snakeBoard[i][j + 1] <= -1)
                snakeBoard[i][j] = -2;
        }
    }
}

//stage�� �� �߰�
//  1 : �߰�����
//  2 : �߾� ���κ� 1��
//  3 : �߾� ���ں� 1��
//  4 : �߾� ���ں� 1�� + ���ϴ� �� �� 2�� (�� 5��)
//  5 : �߾� ���ں� 1�� + �� ���
void addStageWall(int stage) {
    if (stage < 2) return;
    for (int i = 0; i < 5; i++) {
        snakeBoard[snakeBoardY / 2 - i][snakeBoardX / 2] = -1;
        snakeBoard[snakeBoardY / 2 + i][snakeBoardX / 2] = -1;
        if (stage > 2) {
            snakeBoard[snakeBoardY / 2][snakeBoardX / 2 - i] = -1;
            snakeBoard[snakeBoardY / 2][snakeBoardX / 2 + i] = -1;
        }
        if (stage > 3) {
            snakeBoard[1+i][9] = -1;
            snakeBoard[1+i][snakeBoardX-10] = -1;
            snakeBoard[snakeBoardY-2-i][9] = -1;
            snakeBoard[snakeBoardY-2-i][snakeBoardX - 10] = -1;
        }
    }
    if (stage > 4) {
        for (int i = 0; i < snakeBoardY; i++) {
            for (int j = 0; j < 10; j++) {
                snakeBoard[i][j] = -1;
                snakeBoard[i][snakeBoardX-1-j] = -1;
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < snakeBoardX; j++) {
                snakeBoard[i][j] = -1;
                snakeBoard[snakeBoardY-1-i][j] = -1;
            }
        }
    }
}

// ������ ���� ����
void locateItem(int x) {
    int itemY, itemX, poisonY, poisonX;
    for (int m = 0; m < snakeBoardY; m++) {
        for (int n = 0; n < snakeBoardX; n++) {
            if (snakeBoard[m][n] == x)
                snakeBoard[m][n] = 0;
        }
    }
    do {
        itemY = getRandomNumber(18);
        itemX = getRandomNumber(42);
    } while (snakeBoard[itemY][itemX] != 0);
    snakeBoard[itemY][itemX] = x;
}


void setGate() {
    //���������� �̿��� Gate ����
    int addGate1, addGate2;
    int countWall = 0; //Gate ���������� �̿��ϱ� ���� ���� ���� ���� �߰�
    for (int i = 0; i < snakeBoardY; i++) {
        for (int j = 0; j < snakeBoardX; j++) {
            if (snakeBoard[i][j] == -1)
                countWall++;
        }
    }
    do {
        addGate1 = getRandomNumber(countWall - 1);
        addGate2 = getRandomNumber(countWall - 1);
    } while (addGate1 == addGate2);
    for (int m = 0; m < snakeBoardY; m++) {
        for (int n = 0; n < snakeBoardX; n++) {
            if (snakeBoard[m][n] == -1 && addGate1 == 0) {
                snakeBoard[m][n] = 5;
                addGate1--;
                gate1X = n, gate1Y = m;
                break;
            }
            else if (snakeBoard[m][n] == -1)
                addGate1--;
            else
                continue;
        }
    }
    for (int k = 0; k < snakeBoardY; k++) {
        for (int l = 0; l < snakeBoardX; l++) {
            if (snakeBoard[k][l] == -1 && addGate2 == 0) {
                snakeBoard[k][l] = 6;
                addGate2--;
                gate2X = l, gate2Y = k;
                break;
            }
            else if (snakeBoard[k][l] == -1)
                addGate2--;
            else
                continue;
        }
    }
}

//snake �̵�
// status = 0 or etc : �⺻ �̵�
// status = 1 : ���� ���� �� �̵�
// status = -1 : ���� ���� �� �̵�
void moveSnake(int key, int status, int enterGate) {
    int tempY = snake[0][0];
    int tempX = snake[0][1];

    int temp2X, temp2Y;

    // gate �̵�
    if (enterGate > 0 && enterGate <= 4) {
        tempY = snake[0][0];
        tempX = snake[0][1];

        snakeBoard[tempY][tempX] = 0;
        snake[0][0] = gate2Y + nextY(enterGate);
        snake[0][1] = gate2X + nextX(enterGate);

        for (int i = 1; i < snakeLength; i++) {
            snakeBoard[snake[i][0]][snake[i][1]] = 0;
            temp2Y = snake[i][0];
            temp2X = snake[i][1];
            snake[i][0] = tempY;
            snake[i][1] = tempX;
            tempY = temp2Y;
            tempX = temp2X;
        }
        setKey(enterGate);
    }
    else if (enterGate > 4) {
        tempY = snake[0][0];
        tempX = snake[0][1];

        snakeBoard[tempY][tempX] = 0;
        snake[0][0] = gate1Y + nextY(enterGate - 4);
        snake[0][1] = gate1X + nextX(enterGate - 4);

        for (int i = 1; i < snakeLength; i++) {
            snakeBoard[snake[i][0]][snake[i][1]] = 0;
            temp2Y = snake[i][0];
            temp2X = snake[i][1];
            snake[i][0] = tempY;
            snake[i][1] = tempX;
            tempY = temp2Y;
            tempX = temp2X;
        }
        setKey(enterGate - 4);
    }

    else {
        snakeBoard[tempY][tempX] = 0;
        snake[0][0] += nextY(key);
        snake[0][1] += nextX(key);


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
            locateItem(3); // ������ ���� �� ���ġ
        }
        else if (status == -1 && snakeLength > 1) { //���� ����
            snake[snakeLength - 1] = NULL;
            snakeLength--;
            locateItem(4); // ������ ���� �� ���ġ
        }
    }

    drawSnake();
}

//�ʱ� snake ��ġ �� ���� ����
void initSnake() {
    snake = new int* [maxSnakeLength];
    snakeLength = 1;

    snake[0] = new int[2];
    snake[0][0] = snakeBoardY / 2;
    snake[0][1] = 10;

    moveSnake(2, 1,0);
    moveSnake(2, 1,0);
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

// Gate ���� �� ��Ȳ�� �´� Ű ����
// �ܰ� Gate �� ��� ���� ����
// ���ο� ������ Gate�� ��� ������, ����, ����, �ݴ���� ������ ����
int enterGate(int key, int gate1X, int gate1Y, int gate2X, int gate2Y) {
    int leftKey = (key == 1) ? 4 : key - 1;
    int rightKey = (key == 4) ? 1 : key + 1;
    int reverseKey = (key + 2 > 4) ? key - 2 : key + 2;

    int y = snake[0][0] + nextY(key);
    int x = snake[0][1] + nextX(key);

    if (snakeBoard[y][x] == snakeBoard[gate1Y][gate1X] && (gate2Y == 0 || gate2X == 0 || gate2Y == snakeBoardY - 1 || gate2X == snakeBoardX - 1)) {
        if (gate2Y == 0)
            return 3;
        else if (gate2X == 0)
            return 2;
        else if (gate2Y == snakeBoardY - 1)
            return 1;
        else
            return 4;
    }

    else if (snakeBoard[y][x] == snakeBoard[gate2Y][gate2X] && (gate1Y == 0 || gate1X == 0 || gate1Y == snakeBoardY - 1 || gate1X == snakeBoardX - 1)) {
        if (gate1Y == 0)
            return 3 + 4;
        else if (gate1X == 0)
            return 2 + 4;
        else if (gate1Y == snakeBoardY - 1)
            return 1 + 4;
        else
            return 4 + 4;
    }

    else if (snakeBoard[y][x] == snakeBoard[gate1Y][gate1X]) {
        if (snakeBoard[gate2Y + nextY(key)][gate2X + nextX(key)] == 0)
            return key;
        else if (snakeBoard[gate2Y + nextY(rightKey)][gate2X + nextX(rightKey)] == 0)
            return rightKey;
        else if (snakeBoard[gate2Y + nextY(leftKey)][gate2X + nextX(leftKey)] == 0)
            return leftKey;
        else
            return reverseKey;
    }
    else if (snakeBoard[y][x] == snakeBoard[gate2Y][gate2X]) {
        if (snakeBoard[gate1Y + nextY(key)][gate1X + nextX(key)] == 0)
            return key + 4;
        else if (snakeBoard[gate1Y + nextY(rightKey)][gate1X + nextX(rightKey)] == 0)
            return rightKey + 4;
        else if (snakeBoard[gate1Y + nextY(leftKey)][gate1X + nextX(leftKey)] == 0)
            return leftKey + 4;
        else
            return reverseKey + 4;
    }
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
    for (int i = 0; i < snakeBoardY; i++) {
        for (int j = 0; j < snakeBoardX; j++) {
            if (snakeBoard[i][j] == -2 || snakeBoard[i][j] == -1) {
                wattron(win,COLOR_PAIR(2));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(2));
            }
            else if (snakeBoard[i][j] == 0) {
                wattron(win,COLOR_PAIR(1));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(1));
            }
            else if (snakeBoard[i][j] == 1) {
                wattron(win,COLOR_PAIR(7));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(7));
            }
            else if (snakeBoard[i][j] == 2) {
                wattron(win,COLOR_PAIR(4));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(4));
            }
            else if (snakeBoard[i][j] == 3) {
                wattron(win,COLOR_PAIR(3));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(3));
            }
            else if (snakeBoard[i][j] == 4) {
                wattron(win,COLOR_PAIR(5));
                mvwprintw(win, i + 1, j+2, " ");
                wattroff(win,COLOR_PAIR(5));
            }
            else if (snakeBoard[i][j] == 5 || snakeBoard[i][j] == 6) {
                attron(COLOR_PAIR(6));
                mvwprintw(win, i + 1, j+2, "@");
                attroff(COLOR_PAIR(6));
            }
        }
    }
    wrefresh(win);
}

void printScore(int stage, int score) {
    string stringStage, stringScore;
    stringStage = to_string(stage);
    stringScore = to_string(score);
    mvprintw(7, 65, stringStage.c_str());
    mvprintw(15, 65, stringScore.c_str());
    refresh();
}


int main()
{
    WINDOW* snakeWin;
    initscr();
    resize_term(25, 82);
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_YELLOW);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    init_pair(5, COLOR_WHITE, COLOR_RED);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_CYAN);
    border('*', '*', '*', '*', '*', '*', '*', '*');
    mvprintw(1, 1, "Snake Game");
    mvprintw(7, 55, "Stage : ");
    mvprintw(11, 55, "Require : 5");
    mvprintw(15, 55, "Score : ");
    refresh();


    snakeWin = newwin(21, 47, 3, 2);
    wbkgd(snakeWin, COLOR_PAIR(1));
    wattron(snakeWin, COLOR_PAIR(1));
    
    bool isFail = false;
    bool isFinalStage = false;
    int i = 0;

    for (int stage = 1; stage < 6; stage++) {
        if (stage == 3) ticUnit = 250;  //3�ܰ���� �ӵ� 2�� ����
        if (stage == 5) isFinalStage = true;
        mvwprintw(snakeWin, snakeBoardY / 2 - 1, 4, "Press space to start...");
        wrefresh(snakeWin);
        getchar();

        int tic = 0;
        setKey(2); // setKey�� ����
        initSnakeBoard();
        addStageWall(stage);
        setImmuneWall();
        setGate();
        initSnake();
        locateItem(3);
        locateItem(4);

        while (1) {
            Sleep(timeUnit);
            key = keyState(key);
            if (key < 0) {
                isFail = true;
                break; //�ݴ���� �Է� or esc �Է½� ����
            }

            // ���� �ð� ���� ������ ���ġ
            if (i % 500 == 499)
                locateItem(3);
            if (i % 400 == 399)
                locateItem(4);

            if (tic >= ticUnit) {
                if (isConflict(key)) {
                    isFail = true;
                    break;  //�� or ���� �浹�� ����
                }
                moveSnake(key, checkItem(key), enterGate(key, gate1X, gate1Y, gate2X, gate2Y)); //Gate �߰�
                printSnakeBoard(snakeWin);
                printScore(stage, snakeLength - 3);
                tic = 0;
            }
            i++;

            tic += timeUnit;
            if (snakeLength > 6 + stage && stage < 5) break; // �̼� ���� (�䱸���� 5 -> 4 + stage)
            if (snakeLength < 2) {
                isFail = true;
                break;
            }
        }
        if (isFail) break;
        mvwprintw(snakeWin, snakeBoardY / 2 - 1, 4, "Success!!");
        wrefresh(snakeWin);
        getchar();
    }
    
    if(!isFinalStage)
        mvwprintw(snakeWin, snakeBoardY / 2 - 1, 4, "Fail...");
    else
        mvwprintw(snakeWin, snakeBoardY / 2 - 1, 4, "All Clear!!");
    wrefresh(snakeWin);
    getchar();

    delwin(snakeWin);
    endwin();


    return 0;
}

