// gcd.cpp 
#include <iostream> 
#include <string>
#include "battle.h"
#include <map>
#include<conio.h>
#include "structures.h"
#include <termio.h>

int _getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, chr;

    if (tcgetattr(fd, &tm) < 0) return -1;

    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0) return -1;

    char = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) return -1;

    return char;
}
using namespace std;



map<string, Ability> skills; //store player's ablity
map<string, Monster> enemy; //store enemy's data

bool battle(Hero& Ian, bool boss, string Curskills[4]) { //return 0: player died, return 1: player won
	srand(time(NULL));
	int PP[4];
	for (int i = 0; i < 4; i++) {
		if (Curskills[i] != "NA") {
			PP[i] = skills[Curskills[i]].mana;
		}
		else { PP[i] = 0; }

	}// import/refresh the mana(how many times can be used)
	map<string, int> stateP; //player's state in battle
	stateP["Toxicosis"] = 0; // -5 HP
	stateP["Healed"] = 0; // +3 HP
	stateP["Enraged"] = 0; // +5 ATK
	stateP["Freezed"] = 0; // skip 1 turn
	stateP["Weakened"] = 0; // -3 ATK
	stateP["Bleeding"] = 0; // - 3 HP
	map<string, int> stateE; //player's state in battle
	stateE["Toxicosis"] = 0; // -5 HP
	stateE["Healed"] = 0; // +3 HP
	stateE["Enraged"] = 0; // +5 ATK
	stateE["Freezed"] = 0; // skip 1 turn
	stateE["Weakened"] = 0; // -3 ATK
	stateE["Bleeding"] = 0; // - 3 HP
	int enemyNum = rand() % 3 + 1, enemyCode; //generate 1-3 random enemies to fight
	string* ptr;
	if (boss == 1) {//store the generate enemies
		ptr = new string[enemyNum + 1];
	}
	else {
		ptr = new string[enemyNum];// if have boss, add the boss at the end of the enemyList
	}
	map<string, Monster>::iterator itr;
	cout << "You need to fight with: ";
	for (int i = 0; i < enemyNum; i++) {//generating
		itr = enemy.begin();
		enemyCode = rand() % 11;
		if (enemyCode == 9) {
			enemyCode = 12;
		}
		else if (enemyCode == 6) {
			enemyCode = 11;
		}
		for (int j = 0; j < enemyCode; j++) { itr++; }
		cout << (*itr).first << " ";
		ptr[i] = (*itr).first;
	}


	if (boss == 1) {// add the boss
		enemyCode = rand() % 2;
		if (enemyCode == 0) { ptr[enemyNum] = "PitLord"; }
		else { ptr[enemyNum] = "VoidTerror"; }
		cout << "and the Boss: " << ptr[enemyNum] << endl;
	}
	cout << "in this battle" << endl;
	char input;



	for (int i = 0; i < (enemyNum + boss); i++) {//fight!!!
		cout << endl << "\t\tNow you are fight with " << ptr[i] << endl;
		Monster Ene = enemy[ptr[i]];
		while (Ene.HP > 0 && Ian.CurHP > 0) {//leave the while loop when palyer beat an enemy or player die
			cout << endl << "\t\tYour HP :" << Ian.CurHP << "\t\t\tEnemy HP :" << Ene.HP << endl;
			if (PP[0] == 0 && PP[1] == 0 && PP[2] == 0 && PP[3] == 0) {// if running out all pp, lose the game
				cout << "\tYou lose the battle because all the PP are used up" << endl;
				return 0;
			}
			if (stateP["Freezed"] == 0) {//check freeze
				cout << "\t\tPlease press: " << endl;
				if (PP[0] != 0) { cout << "\t1 to use " << Curskills[0] << "  PP: " << PP[0] << endl; }
				if (PP[1] != 0) { cout << "\t2 to use " << Curskills[1] << "  PP: " << PP[1] << endl; }
				if (PP[2] != 0) { cout << "\t3 to use " << Curskills[2] << "  PP: " << PP[2] << endl; }
				if (PP[3] != 0) { cout << "\t4 to use " << Curskills[3] << "  PP: " << PP[3] << endl; }
				cout << "Tips: PP is how many times you can use the skills in the battle" << endl;
				input = _getch();// get valid input
				while (input != '1' && input != '2' && input != '3' && input != '4') {
					cout << "\t\t\tOh! That's an invalid input. Please follow the instrucion!" << endl;
					input = _getch();
				}
				if (PP[input - 49] > 0) {// player turn
					cout << "You used the " << Curskills[input - 49] << endl;
					Ene.HP -= (skills[Curskills[input - 49]].damage + Ian.ATK);
					PP[input - 49] -= 1;
					Ian.CurHP = min(Ian.CurHP + (skills[Curskills[input - 49]].heal), Ian.MaxHP);
					if (skills[Curskills[input - 49]].Peff != "NA") {
						stateP[skills[Curskills[input - 49]].Peff] += 1;
					}
					if (skills[Curskills[input - 49]].Eeff != "NA") {
						stateE[skills[Curskills[input - 49]].Eeff] += 1;
					}
				}
				else {
					cout << "\t\t\tOh! That's an invalid input. Please follow the instrucion!" << endl;
					continue;
				}
			}
			else {
				cout << "\tYou are freezed, skip this turn" << endl;//check freezing
				stateP["Freezed"] -= 1;
			}
			if (Ian.CurHP <= 0) {//check HP
				cout << "\t\tYou died..." << endl;
				return 0;
			}

			if (Ene.HP <= 0) {//check HP
				cout << "\t\tYou won this round!" << endl;
				break;
			}

			if (stateE["Freezed"] == 0) {// enemy turn
				Ian.CurHP -= Ene.ATK;
				cout << "\t You are attacked by the enemy!" << endl;
				cout << " testing HPs  " << Ene.HP << Ian.CurHP << endl;
				if (Ene.Peff != "NA") {
					stateP[Ene.Peff] += 1;
				}
				if (Ene.Eeff != "NA") {
					stateE[Ene.Eeff] += 1;
				}
			}
			else {//check freezing
				stateE["Freezed"] -= 1;
				cout << "the enemy was freezed, it can't attack you in this turn" << endl;
			}

			if (stateP["Toxicosis"] != 0) {//check state of player
				Ian.CurHP -= 5;
				cout << "You are toxicosis, -5 HP" << endl;
				stateP["Toxicosis"] -= 1;
			}
			if (stateP["Healed"] != 0) {
				Ian.CurHP = min(Ian.CurHP + 3, Ian.MaxHP);
				cout << "You are healed, +3 HP" << endl;
				stateP["Healed"] -= 1;
			}
			if (stateP["Enraged"] != 0) {
				Ian.ATK += 5;
				cout << "You are Enraged, +5 ATK" << endl;
				stateP["Enraged"] -= 1;
			}
			if (stateP["Weakened"] != 0) {
				Ian.ATK -= 3;
				cout << "You are Weakened, -3 ATK" << endl;
				stateP["Weakened"] -= 1;
			}
			if (stateP["Bleeding"] != 0) {
				Ian.CurHP -= 3;
				cout << "You are Bleeding, -3 HP" << endl;
				stateP["Bleeding"] -= 1;
			}

			if (stateE["Toxicosis"] != 0) {//check state of enemy
				Ene.HP -= 5;
				cout << "Enemy is Bleeding, -5 HP" << endl;
				stateE["Toxicosis"] -= 1;
			}
			if (stateE["Healed"] != 0) {
				Ene.HP += 3;
				cout << "Enemy is Healed, -5 HP" << endl;
				stateE["Healed"] -= 1;
			}
			if (stateE["Enraged"] != 0) {
				Ene.ATK += 5;
				cout << "Enemy is Enraged, +5 ATK" << endl;
				stateE["Enraged"] -= 1;
			}
			if (stateE["Weakened"] != 0) {
				Ene.ATK -= 3;
				cout << "Enemy is Weakened, -3 ATK" << endl;
				stateE["Weakened"] -= 1;
			}
			if (stateP["Bleeding"] != 0) {
				Ene.HP -= 3;
				cout << "Enemy is Bleeding, -3 HP" << endl;
				stateP["Bleeding"] -= 1;
			}
			if (Ian.CurHP <= 0) {//check HP
				cout << "\t\tYou died..." << endl;
				return 0;
			}
			if (Ene.HP <= 0) {//check HP
				cout << "\t\tYou won this round!" << endl;
				break;
			}

		}
	}
	cout << "you defeated the Enemies";
	delete[] ptr;
	return 1;
}