#include <iostream>
#include <curses.h>
#include <panel.h>
#include <Windows.h>
#include <string>

using namespace std;

int timeUnit = 50;  //입력 단위
int ticUnit = 500;  //1tic당 ms
int key; //key 선언위치 변경

int snakeBoardX = 43;   //board 가로길이
int snakeBoardY = 19;   //board 세로길이
int maxSnakeLength = snakeBoardX * snakeBoardY; //snake 최대길이

int** snakeBoard, ** snake;
int snakeLength = 0;    //현재 snake 길이
int gate1X, gate1Y, gate2X, gate2Y; // 2개의 Gate 좌표변수 추가

// setKey 추가
void setKey(int setKey) {
    key = setKey;
}

//랜덤 난수 출력 (0에서 number까지의 난수 출력)
int getRandomNumber(int number) {
    srand((unsigned int)time(NULL));
    int num = rand() % number;
    return num;
}

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
// -2 : Gate 없는 벽 (Immune Wall)
// -1 : 벽 (진행 불가능)
//  0 : 빈 공간(진행 가능)
//  1 : snake 꼬리
//  2 : snake 머리
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

//ImmuneWall 설정 메서드 추가(아직 시행해보지 않음)
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

//stage별 벽 추가
//  1 : 추가없음
//  2 : 중앙 세로벽 1개
//  3 : 중앙 십자벽 1개
//  4 : 중앙 십자벽 1개 + 상하단 벽 각 2개 (총 5개)
//  5 : 중앙 십자벽 1개 + 맵 축소
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

// 아이템 랜덤 출현
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
    //랜덤난수를 이용한 Gate 생성
    int addGate1, addGate2;
    int countWall = 0; //Gate 랜덤난수를 이용하기 위한 벽의 개수 변수 추가
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

//snake 이동
// status = 0 or etc : 기본 이동
// status = 1 : 길이 증가 및 이동
// status = -1 : 길이 감소 및 이동
void moveSnake(int key, int status, int enterGate) {
    int tempY = snake[0][0];
    int tempX = snake[0][1];

    int temp2X, temp2Y;

    // gate 이동
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
        if (status == 1) { //길이 증가
            snake[snakeLength] = new int[2];
            snake[snakeLength][0] = tempY;
            snake[snakeLength][1] = tempX;
            snake[snakeLength + 1] = NULL;
            snakeLength++;
            locateItem(3); // 아이템 습득 후 재배치
        }
        else if (status == -1 && snakeLength > 1) { //길이 감소
            snake[snakeLength - 1] = NULL;
            snakeLength--;
            locateItem(4); // 아이템 습득 후 재배치
        }
    }

    drawSnake();
}

//초기 snake 위치 및 길이 설정
void initSnake() {
    snake = new int* [maxSnakeLength];
    snakeLength = 1;

    snake[0] = new int[2];
    snake[0][0] = snakeBoardY / 2;
    snake[0][1] = 10;

    moveSnake(2, 1,0);
    moveSnake(2, 1,0);
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

// Gate 출입 시 상황에 맞는 키 리턴
// 외각 Gate 일 경우 방향 고정
// 내부에 생성된 Gate일 경우 정방향, 우측, 좌측, 반대방향 순으로 진행
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

//다음 칸 진행시 충돌 여부 확인(꼬리 or 벽 충돌)
bool isConflict(int key) {
    int y = snake[0][0] + nextY(key);
    int x = snake[0][1] + nextX(key);
    return snakeBoard[y][x] == 1 || snakeBoard[y][x] == -1;
}

//window에 board 출력
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
        if (stage == 3) ticUnit = 250;  //3단계부터 속도 2배 증가
        if (stage == 5) isFinalStage = true;
        mvwprintw(snakeWin, snakeBoardY / 2 - 1, 4, "Press space to start...");
        wrefresh(snakeWin);
        getchar();

        int tic = 0;
        setKey(2); // setKey로 변경
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
                break; //반대방향 입력 or esc 입력시 종료
            }

            // 일정 시간 이후 아이템 재배치
            if (i % 500 == 499)
                locateItem(3);
            if (i % 400 == 399)
                locateItem(4);

            if (tic >= ticUnit) {
                if (isConflict(key)) {
                    isFail = true;
                    break;  //벽 or 꼬리 충돌시 종료
                }
                moveSnake(key, checkItem(key), enterGate(key, gate1X, gate1Y, gate2X, gate2Y)); //Gate 추가
                printSnakeBoard(snakeWin);
                printScore(stage, snakeLength - 3);
                tic = 0;
            }
            i++;

            tic += timeUnit;
            if (snakeLength > 6 + stage && stage < 5) break; // 미션 수정 (요구점수 5 -> 4 + stage)
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

