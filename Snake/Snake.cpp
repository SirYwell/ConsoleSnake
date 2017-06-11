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
#define KEY_CLOSE 27

// Richtungen
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

// Farben
#define MAIN_COLOR 0x27
#define BORDER_COLOR 0x10
#define ITEM_COLOR 0xB0
#define SNAKE_HEAD_COLOR 0xD0
#define SNAKE_BODY_COLOR 0xE0

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

int lastKey = -1; //Zuletzt gedrückte Taste
list<COORD> coords;
COORD item;
bool alive = true;
int score = 0;


void setColor(int color)
{
	SetConsoleTextAttribute(console, color);
}
/* Nicht benötigt, nur für Farbgebungs-Testzwecke
void showColor(int color)
{
	SetConsoleTextAttribute(console, color);
	cout << "Test " << color << endl;
}/**/

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
void drawField()
{
	system("color 27"); // Hauptfarbe (27: hellgrau auf dunkelgrün)
	setCursorPosition(0, 0);
	setColor(BORDER_COLOR);
	cout << "                                                                                  ";
	for (int l = 1; l < 29; l++)
	{
		setCursorPosition(0, l);
		cout << " ";
		setCursorPosition(81, l);
		cout << " ";
	}
	setCursorPosition(0, 29);
	cout << "                                                                                  ";
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
		c.X = 0;
		c.Y = 0;
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
		case KEY_CLOSE:
			result = -2;
			break;
		default:
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
	setColor(ITEM_COLOR);
	cout << " ";
}

void gameOver()
{
	setColor(MAIN_COLOR);
	for (int o = 0; o < 7; o++)
	{
		setCursorPosition(85, 5);
		cout << "GAME OVER!";
		Sleep(300);
		setCursorPosition(85, 5);
		cout << "          ";
		Sleep(100);
	}
	setCursorPosition(85, 5);
	cout << "GAME OVER!";
	alive = false;
	return;
}

// Nächster "Schritt"
void go(int direction)
{
	COORD next = getNextCoord();
	for (list<COORD>::iterator it = coords.begin(); it != coords.end(); it++) {
		if (it->X == next.X && it->Y == next.Y) {
			gameOver();
			return;
		}
	}
	coords.push_front(next);
	if (next.X == item.X && next.Y == item.Y) // Überprüft, ob an der nächsten Position das Item ist
	{
		score++;
		generateItem(); // Generiert neues Item
	}
	else if (next.Y == 0 || next.Y == 29 || next.X == 0 || next.X == 81) // Uberprüft auf Umrandung -> Game Over
	{
		gameOver();
		return;
	}
	else {
		setCursorPosition(coords.back().X, coords.back().Y); // löscht letztes Zeichen der Schlange
		setColor(MAIN_COLOR);
		cout << " ";
		coords.pop_back();
	}
	setCursorPosition(coords.front());
	setColor(SNAKE_HEAD_COLOR);
	cout << " "; // Schreibe den Kopf auf's Spielfeld
	if (coords.size() > 1) //Schreibe den Körper, falls vorhanden
	{
		COORD temp = coords.front();
		coords.pop_front();
		setCursorPosition(coords.front());
		setColor(SNAKE_BODY_COLOR);
		cout << " ";
		coords.push_front(temp);
	}
}

void start()
{
	setColor(MAIN_COLOR);
	setCursorPosition(85, 7);
	cout << "ESC zum Stoppen";
	while (alive)
	{
		setCursorPosition(85, 2);
		setColor(MAIN_COLOR);
		cout << "Score: " << score;
		Sleep(125);
		int key = getNextKey();
		/* Überprüft, ob WASD gedrückt wurde und falls ja,
		ob die Richtungen addiert und geteilt durch 2 einen Restwert
		von nicht 0 ergeben. Dadurch kann man nicht in die
		entgegengesetzte Richtung.
		*/
		if (key != -1 && ((key + lastKey) % 2 != 0))
		{
			lastKey = key; // Update falls neue Richtungstaste gerdrückt wurde
		}
		if (key == -2)
		{
			gameOver();
			return;
		}
		go(lastKey);
	}
}


// Warte, bis der Spieler ein Taste (WASD) drückt
void waitUntilStart() 
{
	setColor(MAIN_COLOR);
	setCursorPosition(85, 7);
	cout << "ESC zum Beenden";
	int key;
	while (alive)
	{
		int key = getNextKey();
		if (key != -1)
		{
			if (key == -2)
			{
				exit(key);
				return;
			}
			lastKey = key;
			start();
		}
	}
	alive = !alive;
	waitUntilStart();
}

int main()
{
	system("title Snake");
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = 20;
	SetConsoleCursorInfo(console, &cursor);
	drawField();
	//showColor(72);
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
	setColor(SNAKE_HEAD_COLOR);
	cout << " ";
	coords.push_front(coord);
	generateItem();
	waitUntilStart();
	return 0;
}

