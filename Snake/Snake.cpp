#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <list>
#include <random>

// Tasten (wasd)
#define KEY_UP 119
#define KEY_DOWN 115
#define KEY_LEFT 97
#define KEY_RIGHT 100

// Richtungen
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

int lastKey = -1; //Zuletzt gedrückte Taste
list<COORD> coords;
COORD item;
bool alive = true;
int score = 0;

/* Nicht benötigt, nur für Farbgebungs-Testzwecke
void showColor(int color)
{
	SetConsoleTextAttribute(console, color);
	cout << "Test " << color << endl;
}*/

void setCursorPosition(COORD coord)
{
	SetConsoleCursorPosition(console, coord);
}

void setCursorPosition(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	setCursorPosition(coord);
}

// Zeichnet die Spielfeldumrandung und legt die Farbgebung fest
void drawField(int height, int width)
{
	system("color 27");
	setCursorPosition(0, 0);
	cout << "+--------------------------------------------------------------------------------+";
	for (int l = 1; l < 29; l++)
	{
		setCursorPosition(0, l);
		cout << "|";
		setCursorPosition(81, l);
		cout << "|";
	}
	setCursorPosition(0, 29);
	cout << "+--------------------------------------------------------------------------------+";
}

// Gibt jeweilige Koordinate in entsprechende Richtung zurück
// Die Koordinate wird kopiert, da sie sich sonst mitverändert
/*-------------------------*/
COORD goRight() 
{
	COORD copy = coords.front();
	copy.X--;
	return copy;
}

COORD goLeft()
{
	COORD copy = coords.front();
	copy.X++;
	return copy;
}

COORD goUpper()
{
	COORD copy = coords.front();
	copy.Y--;
	return copy;
}

COORD goLower()
{
	COORD copy = coords.front();
	copy.Y++;
	return copy;
}
/*------------------------*/

// Gibt nächste Koordinate in gegebener Richtung zurück
// "Umwandlung" der Richtungszahl in Koordinate
COORD getNextCoord()
{
	COORD c;
	switch (lastKey)
	{
	case UP:
		c = goUpper();
		break;
	case DOWN:
		c = goLower();
		break;
	case RIGHT:
		c = goRight();
		break;
	case LEFT:
		c = goLeft();
		break;
	default:
		break;
	}
	return c;
}

// Überprüft auf gedrückte Taste (WASD), sonst -1
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

// Schreibt das Item an eine zufallsgenerierte Position
void generateItem() {
	std::random_device rd;
	std::mt19937 rng(rd());    
	int min = 1;
	int max = 79;
	std::uniform_int_distribution<int> cgen(min, max); 
	auto col = cgen(rng);
	max = 28;
	std::uniform_int_distribution<int> rgen(min, max);
	auto row = rgen(rng);
	item.X = col;
	item.Y = row;
	setCursorPosition(item);
	cout << "#";
}

// Warte, bis der Spieler ein Taste (WASD) drückt
void waitUntilStart() 
{
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

// Nächster "Schritt"
void go(int direction)
{
	COORD next = getNextCoord();
	coords.push_front(next);
	if (next.X == item.X && next.Y == item.Y) // Überprüft, ob an der nächsten Position das Item ist
	{
		score++;
		generateItem(); // Generiert neues Item
	}
	else if (next.Y == 0 || next.Y == 29 || next.X == 0 || next.X == 81) // Uberprüft auf Umrandung -> Game Over
	{
		setCursorPosition(84, 5);
		cout << "GAME OVER!";
		alive = false;
		return;
	}
	else { 
		setCursorPosition(coords.back().X, coords.back().Y); // löscht letztes Zeichen der Schlange
		cout << " ";
		coords.pop_back();
	}
	setCursorPosition(coords.front());
	cout << "O";
}

int main()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 20;
	SetConsoleCursorInfo(console, &cursor);
	drawField(30, 100);

	std::random_device rd;     
	std::mt19937 rng(rd());    
	int min = 2;
	int max = 79;
	std::uniform_int_distribution<int> cgen(min, max); // guaranteed unbiased
	auto col =  cgen(rng);
	max = 28;
	std::uniform_int_distribution<int> rgen(min, max);
	auto row = rgen(rng);
	COORD coord;
	coord.X = col;
	coord.Y = row;
	setCursorPosition(1, 1);
	setCursorPosition(coord);
	cout << "O";
	coords.push_front(coord);
	generateItem();
	waitUntilStart();
	while (alive)
	{
		setCursorPosition(85, 2);
		cout << "Score: " << score;
		Sleep(250);
		int key = getNextKey();
		if (key != -1)
		{
			lastKey = key; // Update falls neue Richtungstaste gerdrückt wurde
		}
		go(lastKey);
	}
	while (!alive) {

	}
	return 0;
}

