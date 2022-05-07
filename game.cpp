#include<iostream>
#include<string>
#include<iomanip>
#include<conio.h>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<map>
#include<algorithm>
#include<unistd.h>
using namespace std;

//64*24 size
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


map<string, Monster> enemy; //store enemy's data
map<string, Ability> skills; //store player's ablity

class Hero {
public:
	int MaxHP, CurHP, ATK;
	vector<string> abilities;
};

void imp() {//import data
	Ability tempSkill;
	Monster tempMonster;
	string tName;
	ifstream fin;
	fin.open("ability.txt", ios::in); //import abilities from the files
	if (fin.fail()) {
		cout << "Error in file opening!" << endl;
		exit(1);
	}
	while (fin >> tName) {
		fin >> tempSkill.damage >> tempSkill.heal >> tempSkill.mana;
		fin >> tempSkill.Peff >> tempSkill.Eeff;
		tempSkill.learned = 0;
		skills[tName] = tempSkill;
	}
	fin.close();

	fin.open("monster.txt", ios::in); //import monsters from the files
	if (fin.fail()) {
		cout << "Error in file opening!" << endl;
		exit(1);
	}
	while (fin >> tName) {
		fin >> tempMonster.HP >> tempMonster.ATK >> tempMonster.Peff >> tempMonster.Eeff;
		enemy[tName] = tempMonster;
	}
	fin.close();
}

void ReadInArchive(Hero& Ian, ifstream& fin, int& level, string CurSkills[4]) {
	Ability tempSkill;
	string command;
	while (fin >> command) {
		if (command == "Level")  fin >> level;
		if (command == "MaxHP") fin >> Ian.MaxHP;
		if (command == "CurHP") fin >> Ian.CurHP;
		if (command == "Attack") fin >> Ian.ATK;
		if (command == "Ability") {
			fin >> command;
			while (command != "CurrentSkill") {
				Ian.abilities.push_back(command);
				skills[command].learned = 1;
				fin >> command;
			}
		}
		if (command == "CurrentSkill")
			for (int i = 0; i < 4; i++)
				fin >> CurSkills[i];
	}
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

void Save(Hero Ian, int level, string CurSkills[4]) {
	ofstream fout;
	fout.open("PlayerArchive.txt");
	fout << "Level " << level << endl;
	fout << "MaxHP " << Ian.MaxHP << endl;
	fout << "CurHP " << Ian.CurHP << endl;
	fout << "Attack" << Ian.ATK << endl;
	fout << "Ability" << endl;
	vector<string>::iterator itr;
	itr = Ian.abilities.begin();
	for (itr; itr != Ian.abilities.end(); ++itr) {
		fout << *itr << " ";
	}
	cout << endl;
	fout << "CurrentSkills" << endl;
	for (int i = 0; i < 4; i++) {
		fout << CurSkills[i] << " ";
	}

	fout.close();
}

void dead() {
	ofstream fout;
	fout.open("PlayerArchive.txt");
	fout << "dead";
	fout.close();
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
			else cout << "|  √  |";
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

void changeSkills(string a[4], Hero Ian) {
	system("clear");
	cout << "\t\t\tYour current 4 skills for next fight: \n";
	for (int i = 0; i < 4; ++i) {
		cout << a[i] << "\t";
	}
	cout << endl << endl;
	cout << "\t\t\tAll the skills you learned: \n";
	int t = 0;//total number of the learned skills
	vector<string>::iterator itr = Ian.abilities.begin();
	for (itr; itr != Ian.abilities.end(); ++itr) {
		++t;
		cout << t << ": " << *itr << endl;
	}//show all the learned skills
	if (t <= 4)
		cout << "\t\tYou haven't learned 4 skills yet! \n";
	else {
		cout << "\t\tEnter 4 different numbers of the skills you want use in next fight: \n";
		int s[4];
		bool flag = 0;
		while (flag == 0) {
			flag = 1;
			for (int i = 0; i < 4; ++i) cin >> s[i];
			sort(s, s + 4);

			if (s[3] > t) {
				cout << "\t\t\tInvalid input! Please enter agian!\n";
				flag = 0;//failed
			}
			else {
				for (int i = 0; i < 3; ++i) {
					if (s[i] == s[i + 1]) {
						cout << "\t\t\tDuplicated input! Please enter again!\n";
						flag = 0;//failed
					}
					if (flag == 0) break;
				}
			}
		}//get a vaild input
		int cnt = 0;
		while (cnt < 4) {//change the skills
			t = 0;
			for (itr = Ian.abilities.begin(); itr != Ian.abilities.end(); ++itr) {
				++t;
				if (t == s[cnt]) {
					a[cnt] = *itr;
					cnt++;
					itr = Ian.abilities.end();
					break;
				}
			}
		}
		cout << "\t\tYou have successfully changed you skills!\n";
	}
	sleep(3);
}


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
				cin >> input;// get valid input
				while (input != '1' && input != '2' && input != '3' && input != '4') {
					cout << "\t\t\tOh! That's an invalid input. Please follow the instrucion!" << endl;
					cin >> input;
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

void initialize(Hero& Ian) {
	Ian.MaxHP = 100;
	Ian.CurHP = 100;
	Ian.abilities.push_back("Fireblast");
	Ian.ATK = 0;
}


int main() {
FLAG:
	srand(time(NULL));
	Hero Ian;//The Hero of the game
	string CurSkills[4];//Current skills for the fight
	frontPage();//show the front page
	char key;
	key = GetOption();//Get first option
	int level = 1;
	ifstream fin;
	bool continueAble = 0;
	if (key == '3') { //3:  Exit
		return 0;
	}
	else if (key == '1') {//1: New Game
		fin.open("DefaultArchive.txt");//read the default archive	
		cout << "\t\tStarting a new game...\n";
		sleep(2);
	}
	else if (key == '2') {//2: Continue Game
		fin.open("PlayerArchive.txt");//read the player archive
		string a;
		fin >> a;
		if (a == "dead") {
			cout << "\t\tYou don't have an archive. Starting a new game...\n";
			sleep(2);
			fin.open("DefaultArchive.txt");//read the default archive	
		}
		else {
			cout << "\t\tLoading the last game...\n";
			sleep(2);
		}
	}
	system("clear");
	imp();
	ReadInArchive(Ian, fin, level, CurSkills);//load the game data
	fin.close();

	for (level; level < 6; ++level) {
		Save(Ian, level, CurSkills);//save the data at the beginning of the level
		int x, y;
		x = 4, y = 4;//position of player
		int bx, by; //position of boss
		Room m[10][10];
		bool mapOpen;
		generateMap(m, level, bx, by);//generate a random map
		showMap(m, level, x, y, mapOpen);//show the map
		while (m[bx][by].cleared == 0) {
			cout << "			Press [Q] to quit the game." << endl;
			cout << "			Press [B] to modify your skills." << endl;
			cout << "			Press [W/A/S/D] to move in the map." << endl;
			char key = _getch();
			char curType;// current room type
			bool curCleared;// current room status
			curType = m[x][y].type;
			curCleared = m[x][y].cleared;
			if ((curType == 'm' || curType == 'M' || curType == 'B') && (curCleared == 0)) {//in the monster room
				if (key == 'f') {
					m[x][y].cleared = battle(Ian, m[x][y].type, CurSkills);
				}
				else if (key == 'q') {//quit
					cout << "\t\t\t\tWarning!!\n\t\tYour data in this level won't be saved if you quit now!!\n\t\tPress [Q] to confirm\t\tPress [C] to cancel\n";
					key = _getch();
					system("clear");
					if (key == 'q') goto FLAG;
					if (key == 'c') showRoom(m, x, y);
				}
				else if (key == 'm') {//close or open the map
					if (mapOpen == true) {
						showRoom(m, x, y);
						mapOpen = false;
					}
					else {
						showMap(m, level, x, y, mapOpen);
					}
				}
				else {
					cout << "\t\t\tFighting is doomed!Don't run away!\n";
					m[x][y].cleared = battle(Ian, m[x][y].type, CurSkills);
				}

				if (m[x][y].cleared == 0) {
					system("clear");
					cout << "\n\n\n\n\n\n\t\t\t\t\tYou are dead!";
					dead();
					goto FLAG;
				}
				else {
					system("clear");
					cout << "\n\n\n\n\n\n\t\t\tYou win!\n";
					sleep(4);
					showRoom(m, x, y);
				}
			}
			else {//not in the monster room (or cleared the monster)
				switch (key) {
				case 'm'://close or open the map
					if (mapOpen == true) {
						showRoom(m, x, y);
						mapOpen = false;
					}
					else {
						showMap(m, level, x, y, mapOpen);
					}
					break;
				case 'q'://quit
					cout << "	\t\t\t\tWarning!!\n\t\tYour data in this level won't be saved if you quit now!!\n\t\tPress [Q] to confirm\t\tPress [C] to cancel\n";
					key = _getch();
					system("clear");
					if (key == 'q') goto FLAG;
					if (key == 'c') showRoom(m, x, y);

					//movement in four directions
				case 'w':
					if (x == 0) {
						cout << "\t\t\tOut of the boundary!\n";
						break;
					}
					if (m[x - 1][y].type == 'N') {
						cout << "\t\t\tNot a Through Road!\n";
						break;
					}
					x -= 1;
					showRoom(m, x, y);
					break;
				case 's':
					if (x == 9) {
						cout << "\t\t\tOut of the boundary!\n";
						break;
					}
					if (m[x + 1][y].type == 'N') {
						cout << "\t\t\tNot a Through Road!\n";
						break;
					}
					x += 1;
					showRoom(m, x, y);
					break;
				case 'a':
					if (y == 0) {
						cout << "\t\t\tOut of the boundary!\n";
						break;
					}
					if (m[x][y - 1].type == 'N') {
						cout << "\t\t\tNot a Through Road!\n";
						break;
					}
					y -= 1;
					showRoom(m, x, y);
					break;
				case 'd':
					if (y == 9) {
						cout << "\t\t\tOut of the boundary!\n";
						break;
					}
					if (m[x][y + 1].type == 'N') {
						cout << "\t\t\tNot a Through Road!\n";
						break;
					}
					y += 1;
					showRoom(m, x, y);
					break;

				case 'e':
					if (m[x][y].type == 'H') {//heal up
						Ian.CurHP = Ian.MaxHP;
						system("clear");
						cout << "\t\t\t You are healed up!\n\n\n\t\t\tCurrent HP: " << Ian.MaxHP << endl;
						sleep(4);
						m[x][y].cleared = 1;
						showRoom(m, x, y);
					}
					else if (m[x][y].type == 'A') {//learn the ability
						string tmpAbility;
						map<string, Ability>::iterator itr;
						int tot = 0, code = 0;
						for (itr = skills.begin(); itr != skills.end(); ++itr) tot;//get the total numbers of the abilities
						itr = skills.begin();
						code = rand() % tot;
						for (int i = 0; i < code + 1; ++i) ++itr;
						while ((*itr).second.learned == true) {
							itr = skills.begin();
							code = rand() % tot;
							for (int i = 0; i < code + 1; ++i) ++itr;
						}//roll an unlearned ability randomly
						tmpAbility = (*itr).first;
						skills[tmpAbility].learned = 1;//learned
						system("clear");
						cout << "\t\t\t You have learned this skill called" << tmpAbility << endl;
						sleep(3);
						Ian.abilities.push_back(tmpAbility);//Ian learned this ability;
						m[x][y].cleared = 1;
						showRoom(m, x, y);
					}
					break;
				case 'b'://change skills
					changeSkills(CurSkills, Ian);
					m[x][y].cleared = 1;
					showRoom(m, x, y);
					break;
				default:
					cout << "\t\t\tInvalid input!\n";
					sleep(2);
					showRoom(m, x, y);
				}
			}
		}
	}
  	cout<<"\n\n\n\n\t\t\tCongratulations! You defeated all the monsters!";
  	sleep(10)
	return 0;
}
