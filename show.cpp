// show.cpp
#include<iostream>
#include<iomanip>
#include "show.h"
using namespace std;

void frontPage() {
	string txt;
	cout << setw(64) << setfill('=') << "";
	cout << endl;
	for (int i = 2; i < 24; ++i) {
		cout << "||";
		switch (i) {
		case 6:
			txt = "1: New Game";
			break;
		case 12:
			txt = "2: Continue Game";
			break;
		case 18:
			txt = "3: Exit";
			break;
		default:
			txt = "";
			break;
		}
		cout << setw(20) << setfill(' ') << "";
		cout << left << setw(40) << setfill(' ') << txt;
		cout << "||" << endl;
	}
	cout << setw(64) << setfill('=') << "";
	cout << endl;
}

void generateMap(Room m[10][10], int level, int& bx, int& by) {
	int tot = 8 + 2 * level;//tot number of rooms in a level
	int x = 4, y = 4;//(x,y)is current position
	int dx = 0, dy = 0;
	m[x][y].type = '0';//birthplace
	while (tot) {
		dx = 0; dy = 0;
		int f1 = 0, f2 = 0;
		f1 = rand() % 2;
		f2 = rand() % 2;
		if (f1 == 0) {
			if (f2 == 0) dx = -1;
			else dx = 1;
		}
		else {
			if (f2 == 0) dy = -1;
			else dy = 1;
		}// determine a direction
		if ((0 <= x + dx) && (x + dx < 10) && (0 <= y + dy) && (y + dy < 10)) {
			x += dx; y += dy;
			if (m[x][y].type != 'N')
				while ((0 <= x + dx) && (x + dx < 10) && (0 <= y + dy) && (y + dy < 10)) {
					x += dx; y += dy;
					if (m[x][y].type == 'N')
						break;
				}

			if (tot == 1) {
				m[x][y].type = 'B';
				--tot;
				bx = x;
				by = y;
			}
			else {
				f1 = rand() % 100;
				//ability(25%),heal(35%),monster1(25%),monster2(15%),boss(final one)
				if (0 <= f1 && f1 < 25) {
					m[x][y].type = 'A';
				}
				else if (25 <= f1 && f1 < 60) {
					m[x][y].type = 'H';
				}
				else if (60 <= f1 && f1 < 85) {
					m[x][y].type = 'm';
				}
				else if (85 <= f1 && f1 < 100) {
					m[x][y].type = 'M';
				}
				--tot;
			}
		}
	}
}

void showMap(Room m[10][10], int level, int cx, int cy, bool& mapOpen) {
	mapOpen = true;
	system("clear");
	cout << "			This is the map of level " << level << endl;
	cout << "			+=====+ You are at +=====+" << endl;
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			if (m[i][j].type == 'N') cout << "       ";
			else if (i == cx && j == cy) cout << "+=====+";
			else cout << "+-----+";
		}//first layer
		cout << endl;
		for (int j = 0; j < 10; ++j) {
			char t = m[i][j].type;
			if (t == 'N')cout << "       ";
			else if (m[i][j].cleared == 0) {
				if (t == 'B')
					cout << "| !!! |";
				else if (t == 'M')
					cout << "| ! ! |";
				else if (t == 'm')
					cout << "|  !  |";
				else if (t == 'A')
					cout << "|  ?  |";
				else if (t == 'H')
					cout << "|  *  |";
				else if (t == '0')
					cout << "|     |";
			}
			else cout << "|  ¡Ì  |";
		}//second layer
		cout << endl;
		for (int j = 0; j < 10; ++j) {
			if (m[i][j].type == 'N') cout << "       ";
			else if (i == cx && j == cy) cout << "+=====+";
			else cout << "+-----+";
		}//third layer
		cout << endl;
	}
	cout << "			Press [M] to open/close the map." << endl;
}

void showRoom(Room m[10][10], int x, int y) {
	system("clear");
	string txt[25];
	cout << setw(64) << setfill('=') << "";
	cout << endl;
	if (m[x][y].cleared == 0) {
		switch (m[x][y].type) {
		case '0':
			txt[6] = "This is your birthroom, empty and peace.";
			txt[12] = "Go and defeat the boss!";
			txt[18] = "Try not to die!";
		case 'B':
			txt[6] = "This is the Boss room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
		case 'M':
			txt[6] = "This is a medium monster room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
		case 'm':
			txt[6] = "This is a weak monster room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
		case 'A':
			txt[6] = "This is an learning room.";
			txt[12] = "Press [E] to learn a skill!";
			txt[18] = "It may be powerful!";
		case 'H':
			txt[6] = "This is the healing room.";
			txt[12] = "Press [E] to heal up yourself";
			txt[18] = "You can only use once in this room";
		}
	}
	else {
		txt[12] = "This room is empty.";
	}
	for (int i = 2; i < 24; ++i) {
		cout << "||";
		cout << setw(20) << setfill(' ') << "";
		cout << left << setw(40) << setfill(' ') << txt[i];
		cout << "||" << endl;
	}
	cout << setw(64) << setfill('=') << "";
	cout << endl;
}
