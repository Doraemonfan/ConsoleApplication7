#include "stdafx.h"
#include <graphics.h>
#include <cstring>
#include <time.h>
#include <conio.h>
#pragma comment(lib, "Winmm.lib")

#define Width 500
#define High 400

int gameStatus = 0;  //0 menu , 1 game, 2 over, 3 pause

IMAGE img_bk, img_bd, img_bar_up, img_bar_down;
int bird_x, bird_y;
int bar_top_x, bar_top_y;
int bar_bottom_x, bar_bottom_y;
int fly;
int speed;
int isFail;


void startMenu();
void pauseMenu();
void readRecordFile();
void writeRecordFile();
void startup();
void show();
void updateWithoutInput();
void updateWithInput();
void PlayMusicOnce(CHAR *);
void gameover();

void startMenu() {
	putimage(0, 0, &img_bk);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(30, 0, _T("黑体"));
	outtextxy(Width * 0.3, High * 0.2, "1 新游戏");
	outtextxy(Width * 0.3, High * 0.3, "2 读取游戏存档");
	outtextxy(Width * 0.3, High * 0.4, "3 退出");

	settextcolor(BLUE);
	settextstyle(20, 0, _T("黑体"));
	outtextxy(Width * 0.3, High * 0.6, _T("F键小鸟操控飞动"));
	outtextxy(Width * 0.3, High * 0.65, _T("撞到柱子或落地游戏失败"));

	FlushBatchDraw();
	Sleep(2);

	char input;
	if (_kbhit()) {
		input = _getch();
		if (input == '1') gameStatus = 1;
		else if (input == '2') { readRecordFile(); gameStatus = 1; }
		else if (input == '3') { gameStatus = 2; exit(0); };
	}
}

void pauseMenu() {
	putimage(0, 0, &img_bk);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(30, 0, _T("黑体"));
	outtextxy(Width * 0.3, High * 0.2, _T("1 继续游戏"));
	outtextxy(Width * 0.3, High * 0.3, _T("2 保存"));
	outtextxy(Width * 0.3, High * 0.4, _T("3 退出"));

	settextcolor(BLUE);
	settextstyle(20, 0, _T("黑体"));
	outtextxy(Width * 0.3, High * 0.6, _T("F键操控小鸟飞动"));
	outtextxy(Width * 0.3, High * 0.65, _T("撞到柱子或落地游戏失败"));

	FlushBatchDraw();
	Sleep(2);

	char input;
	if (_kbhit()) {
		input = _getch();
		if (input == '1') gameStatus = 1;
		else if (input == '2') { writeRecordFile(); gameStatus = 3; }
		else if (input == '3') { gameStatus = 2; exit(0); }
	}
}

void writeRecordFile() {
	FILE * fp;
	fopen_s(&fp, ".\\gameRecord.dat", "w");
	fprintf_s(fp, "%d %d %d %d %d %d %d %d",
		bird_x, bird_y, bar_top_x, bar_top_y, bar_bottom_x, bar_bottom_y, speed, isFail);
	fclose(fp);
}

void readRecordFile() {
	FILE * fp;
	fopen_s(&fp, ".\\gameRecord.dat", "r") == EOF;
	fscanf_s(fp, "%d %d %d %d %d %d %d %d",
		&bird_x, &bird_y, &bar_top_x, &bar_top_y, &bar_bottom_x, &bar_bottom_y, &speed, &isFail);
	fclose(fp);
}

void startup() {
	initgraph(Width, High);

	loadimage(&img_bk, _T(".\\picture\\background.jpg"));
	loadimage(&img_bd, _T(".\\picture\\bird.jpg"));
	loadimage(&img_bar_up, _T(".\\picture\\bar_up.jpg"));
	loadimage(&img_bar_down, _T(".\\picture\\bar_down.jpg"));

	bird_x = Width / 5;
	bird_y = High / 2;

	srand((unsigned)time(NULL));

	bar_top_x = Width / 2;
	bar_top_y = 0 - rand() % 26 * 10;

	bar_bottom_x = bar_top_x;
	bar_bottom_y = bar_top_y + 400;

	fly = 0;
	speed = 0;
	isFail = 0;

	BeginBatchDraw();

	while (gameStatus == 0)
		startMenu();

	mciSendString(_T("open .\\music\\background2.mp3 alias bkmusic"), NULL, 0, NULL);
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
}

void PlayMusicOnce(CHAR * fileName) {
	CHAR cmdString1[50] = "open ";
	strcat_s(cmdString1, fileName);
	strcat_s(cmdString1, " alias temp_music");
	mciSendString(_T("close temp_music"), NULL, 0, NULL);
	mciSendString(cmdString1, NULL, 0, NULL);
	mciSendString(_T("play temp_music"), NULL, 0, NULL);
}

void show() {
	while (gameStatus == 3) pauseMenu();

	putimage(0, 0, &img_bk);
	putimage(bar_top_x, bar_top_y, &img_bar_up);
	putimage(bar_bottom_x, bar_bottom_y, &img_bar_down);
	putimage(bird_x, bird_y, &img_bd);

	if (isFail) {
		setbkmode(TRANSPARENT);
		settextcolor(GREEN);
		settextstyle(64, 0, _T("黑体"));
		outtextxy(Width / 5, High / 3, _T("Game over!"));
		FlushBatchDraw();
		system("pause");
		exit(0);
	}
	FlushBatchDraw();
}

void updateWithoutInput() {
	if ((bird_x > bar_top_x && bird_x < bar_top_x + 60)
		&& (bird_y < bar_top_y + 300 || bird_y > bar_bottom_y)) {
		PlayMusicOnce(".\\music\\Hit.mp3");
		isFail = 1;
	}

	if (bird_y > High) {
		PlayMusicOnce(".\\music\\Down.mp3");
		isFail = 1;
	}

	if (fly == 1) {
		speed = 0;
		fly = 0;
		bird_y -= 50;
		PlayMusicOnce(".\\music\\Jump.mp3");
	}
	else if (bird_y < High - 4) {
		speed++;
		bird_y += speed;
		if (speed == 10) 
			PlayMusicOnce(".\\music\\Flow.mp3");
		}
	//if (bird_y < High - 4) bird_y += 2;

	if (bar_top_x < 0) {
		bar_top_x = Width;
		bar_top_y = 0 - rand() % 26 * 10;

		bar_bottom_x = bar_top_x;
		bar_bottom_y = bar_top_y + 400;
	}
	else {
		bar_top_x -= 4;
		bar_bottom_x -= 4;
	}

	Sleep(50);
}

void updateWithInput() {
	CHAR input;
	if (_kbhit()) {
		input = _getch();
		if (input == 'f' && bird_y > 20) fly = 1;
		else if (input == 27) {
			gameStatus = 3;
		}
	}
}

void gameover() {
	EndBatchDraw();
	closegraph();
}

int main() {
	startup();
	for (;;) {
		show();
		updateWithoutInput();
		updateWithInput();
	}
	gameover();
	return 0;
}