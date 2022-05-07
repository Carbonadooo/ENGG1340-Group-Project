// show.h
#ifndef SHOW_H
#define SHOW_H
#include<string>
using namespace std;

class Room {
public:
	char type = 'N';
	int x, y;
	bool cleared = 0;
	string ability = "", monster = "";
};

void frontPage();
void generateMap(Room m[10][10], int level, int& bx, int& by);

#endif