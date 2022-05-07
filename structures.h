// structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H
#include<string>
#include<vector>
using namespace std;

struct Ability {
	int damage, heal, mana, number; // mana: the cost of the skills
	string Peff, Eeff; // the skills that will have some effect on Player or Enemy (Player's effect and Enemy's effect)
	bool learned = 0;
};
struct Monster {
	int HP, ATK;
	string Peff, Eeff; //if player was attacked by the enemy, it will have some effect on Player or Enemy (Player's effect and Enemy's effect)
};
class Room {
public:
	char type = 'N';
	int x, y;
	bool cleared = 0;
	string ability = "", monster = "";
};
class Hero {
public:
	int MaxHP, CurHP, ATK;
	vector<string> abilities;
};

#endif