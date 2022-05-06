#include<iostream>
#include<string>
#include<iomanip>
#include<windows.h>
#include<conio.h>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<map>
using namespace std;

//64*24 size
void frontPage() {
	string txt;
	cout << setw(64) << setfill('=')<<"";
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
				txt = "3: Save & Exit";
				break;
			default:
				txt = "";
				break;
		}
		cout << setw(20) <<setfill(' ') << "";
		cout << left<< setw(40) << setfill(' ') << txt;
		cout << "||" << endl;
	}
	cout << setw(64) << setfill('=')<<"";
	cout << endl;
}

struct Ability{
    int damage, heal, mana, number;
    string Peff, Eeff;
};

struct monster{
    int HP, ATK;
    string Peff, Eeff;
};

class Room {
public:
	char type = 'N';
	int x, y;
	Ability ability;

};


map<string, monster> enemy;  //store enemy's data


class Hero {
	public:
		int HP, ATK;
		map<string, Ability> skills; //store player's ablity
};

void imp(Hero &Ian){//import data
    Ability tempSkill;
    monster tempMonster;
    string tName;
    ifstream fin;
    fin.open("ability.txt", ios::in); //import abilities from the files
    if ( fin.fail() ){
        cout << "Error in file opening!" << endl;
        exit(1);
    }
    while(fin >> tName){
        fin  >> tempSkill.damage >> tempSkill.heal >> tempSkill.mana;
        fin >> tempSkill.Peff >> tempSkill.Eeff >> tempSkill.number;
        Ian.skills[tName] = tempSkill;
    }
    fin.close();

        fin.open("monster.txt", ios::in); //import monsters from the files
    if ( fin.fail() ){
        cout << "Error in file opening!" << endl;
        exit(1);
    }
    while(fin >> tName){
        fin  >> tempMonster.HP >> tempMonster.ATK >> tempMonster.Peff >> tempMonster.Eeff;
        enemy[tName] = tempMonster;
    }
    fin.close();
}


char GetOption() {
	cout << "Enter number to make choice: " << endl;
	char key = _getch();
	while (key != '1' && key != '2' && key != '3') {
		cout << "Invalid input! Enter again!\n";
		key = _getch();
	}
	return key;
}

void generateMap(Room map[10][], int level) {
	int tot = 8 + 2 * level;
	int x = 4, y = 4;
	map[x][y].type = '0';//birthplace

	while (tot) {
		if (map[x][y].type == 'N') {
			srand(time(NULL));
			int dx = 0, dy = 0;
			if (rand() % 2) dx = (rand() % 2) ? -1 : 1;
			else dy = (rand() % 2) ? -1 : 1;
			while (0 <= x + dx && x + dx < 10 && 0 <= y + dy && y + dy < 10) {
				x += dx; y += dy;
				if (map[x][y].type != 'N') continue;
				else {
					int t = rand()%
						//ability(),heal,monster1,monster2,boss(final one)
					map[x][y].type = 'A';

				}
			}

		}
	}
}

int main() {
	Hero Ian;//The Hero of the game
	frontPage();
	char key;
	key = GetOption();//Get first option

	ifstream fin;
	if (key == '3') { //3: Save & Exit
		return 0;
	}
	else if (key == '1') {//1: New Game
		fin.open("DefaultArchive.txt");//read the default archive
	}
	else if (key == '2') {//2: Continue Game
		fin.open("PlayerArchive.txt");//read the player archive
	}
	system("cls");
	if (fin.fail()) { cout << "error"; exit(1); }
	for (int level = 1; level < 6; ++level) {
		Room map[10][10];
		generateMap(map, level);
	}
	return 0;
}
