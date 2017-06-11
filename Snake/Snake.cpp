// SnakeGame.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <conio.h>

//Tasten (wasd)
#define KEY_UP 119
#define KEY_DOWN 115
#define KEY_LEFT 97
#define KEY_RIGHT 100

//Richtungen
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
int lastKey = -1; //Zuletzt gedrückte Taste

void showColor(int color)
{
	SetConsoleTextAttribute(console, color);
	cout << "Test " << color << endl;
}

void go(int direction)
{
	cout << "Ich gehe in Richtung " << direction << endl;
}

void drawField(int height, int width)
{
	system("color 27");
	for (int a = 0; a < height; a++) {
		for (int b = 0; b < width; b++) {
			cout << " ";
		}
		cout << endl;
	}
}

void setCursorPosition(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(console, coord);
}

char getNextKey()
{
	int result = -1;
	if (_kbhit())
	{
		switch (_getch())
		{
		case KEY_UP:
			result = 0;
			break;
		case KEY_DOWN:
			result = 2;
			break;
		case KEY_LEFT:
			result = 1;
			break;
		case KEY_RIGHT:
			result = 3;
			break;
		default:
			result = -1;
			break;
		}
	}
	return result;
}

void waitUntilStart() {
	setCursorPosition(0, 1);
	bool started = false;
	int key = 0;
	while (!started)
	{
		int key = getNextKey();
		if (key != -1)
		{
			lastKey = key;
			started = !started;
		}
	}
}

int main()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 20;
	SetConsoleCursorInfo(console, &cursor);
	/*for(int a = 0x00; a < 0xff; a++)
	{
	showColor(a);
	}*/
	drawField(30, 100);
	int min = 10;
	int max = 90;
	int col = min + (rand() % static_cast<int>(max - min + 1));
	max = 20;
	int row = min + (rand() % static_cast<int>(max - min + 1));
	COORD coord;
	coord.X = col;
	coord.Y = row;
	SetConsoleCursorPosition(console, coord);
	cout << "O";
	waitUntilStart();
	while (true)
	{
		Sleep(500);
		int key = getNextKey();
		if (key != -1)
		{
			lastKey = key; //Update falls neue Richtungstaste gerdrückt wurde
		}
		go(lastKey);
	}
	return 0;
}

