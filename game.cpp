#include<iostream>
#include<string>
#include<iomanip>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<map>
#include<algorithm>
#include<unistd.h>
#include<termio.h>
using namespace std;

int _getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, chr;

    if (tcgetattr(fd, &tm) < 0) return -1;

    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0) return -1;

    chr = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) return -1;

    return chr;
}

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

void ReadInArchive(Hero& Ian, int x, int& level, string CurSkills[4]) {
	Ability tempSkill;
	string command;
  ifstream fin;
  if (x==1) {
    fin.open("DefaultArchive.txt");
    cout<<"\t\tStarting a new game...\n";
    sleep(2);
  }
  else {
    fin.open("PlayerArchive.txt");
    fin>>command;
    if(command=="dead"){
      cout<<"\t\tYou don't have an archive. Starting a new game...\n";
      sleep(2);
      fin.close();
      fin.open("DefaultArchive.txt");
    }
    else{
      cout<<"\t\tLoading the last game.\n";
      sleep(2);
    }
  }
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
		if (command == "CurrentSkill"){
      command="";
			for (int i = 0; i < 4; i++)
				fin >> CurSkills[i];
      }
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

void Save(Hero Ian, int level, string CurSkills[4]) {
	ofstream fout;
	fout.open("PlayerArchive.txt");
  fout << "alive" << endl;
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
	fout << "CurrentSkill" << endl;
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
      break;
		case 'B':
			txt[6] = "This is the Boss room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
      break;
		case 'M':
			txt[6] = "This is a medium monster room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
      break;
		case 'm':
			txt[6] = "This is a weak monster room.";
			txt[12] = "Press [F] to start the fight!";
			txt[18] = "Good luck!";
      break;
		case 'A':
			txt[6] = "This is an learning room.";
			txt[12] = "Press [E] to learn a skill!";
			txt[18] = "It may be powerful!";
      break;
		case 'H':
			txt[6] = "This is the healing room.";
			txt[12] = "Press [E] to heal up yourself";
			txt[18] = "You can only use once in this room";
      break;
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

bool battle(Hero& Ian, bool boss, string Curskills[4],map<string, Ability> skills ,map<string, Monster> enemy ) { //return 0: player died, return 1: player won
	srand(time(NULL));
	int PP[4];
	for (int i = 0; i < 4; i++) {
		if (Curskills[i] != "NA") {
			PP[i] = skills[Curskills[i]].mana;
		}
		else { PP[i] = 0;}

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
		cout << (*itr).first << ", ";
		ptr[i] = (*itr).first;
	}


	if (boss == 1) {// add the boss
		enemyCode = rand() % 2;
		if (enemyCode == 0) { ptr[enemyNum] = "PitLord"; }
		else { ptr[enemyNum] = "VoidTerror"; }
		sleep(5);
		cout << "and the Boss: " << ptr[enemyNum] << endl;
	}

	cout << "in this battle" << endl;
	char input;
	sleep(5);
	system("clear");

	for (int i = 0; i < (enemyNum + boss); i++) {//fight!!!
		cout << endl << "\t\tNow you are fight with " << ptr[i] << endl;
		Monster Ene = enemy[ptr[i]];
		sleep(5);
		system("clear");
		while (Ene.HP > 0 && Ian.CurHP > 0) {//leave the while loop when palyer beat an enemy or player die
			cout << endl << "\t\tYour HP :" << Ian.CurHP << "\t\tEnemy HP :" << Ene.HP << endl;
			if (PP[0] == 0 && PP[1] == 0 && PP[2] == 0 && PP[3] == 0) {// if running out all pp, lose the game
				system("clear");
				cout << "\n\n\t\tYou lose the battle because all the PP are used up" << endl;
				sleep(5);
				cout << "\n\n\n\t\t                game over!" 
				sleep(5);
				return 0;
			}
			if (stateP["Freezed"] == 0) {//check freeze
				cout << "\n\n\n\t\tPlease press: " << endl;
				if (PP[0] != 0) { cout << "1 to use " << setw(19) << Curskills[0] << "  PP: " << PP[0] << endl; }
				if (PP[1] != 0) { cout << "2 to use " << setw(19) << Curskills[1] << "  PP: " << PP[1] << endl; }
				if (PP[2] != 0) { cout << "3 to use " << setw(19) << Curskills[2] << "  PP: " << PP[2] << endl; }
				if (PP[3] != 0) { cout << "4 to use " << setw(19) << Curskills[3] << "  PP: " << PP[3] << endl; }
				cout << "Tips: PP is how many times you can use the skills in the battle" << endl;
				input = _getch();// get valid input
				while (input != '1' && input != '2' && input != '3' && input != '4') {
					system("clear");
					cout << "\t\tOh! That's an invalid input. Please follow the instrucion!" << endl;
					sleep(5);
					system("clear");
					cout << endl << "\t\tNow you are fight with " << ptr[i] << endl;
					cout << "\n\n\n\t\tPlease press: " << endl;
					if (PP[0] != 0) { cout << "1 to use " << setw(19) << Curskills[0] << "  PP: " << PP[0] << endl; }
					if (PP[1] != 0) { cout << "2 to use " << setw(19) << Curskills[1] << "  PP: " << PP[1] << endl; }
					if (PP[2] != 0) { cout << "3 to use " << setw(19) << Curskills[2] << "  PP: " << PP[2] << endl; }
					if (PP[3] != 0) { cout << "4 to use " << setw(19) << Curskills[3] << "  PP: " << PP[3] << endl; }
					cout << "Tips: PP is how many times you can use the skills in the battle" << endl;
					input = _getch();
				}
				if (PP[input - 49] > 0) {// player turn
					system("clear");
					cout << "\n          You used the " << Curskills[input - 49] << endl;
					Ene.HP -= (skills[Curskills[input - 49]].damage + Ian.ATK);
					PP[input - 49] -= 1;
					Ian.CurHP = min(Ian.CurHP + (skills[Curskills[input - 49]].heal), Ian.MaxHP);
					if (skills[Curskills[input - 49]].Peff != "NA") {
						stateP[skills[Curskills[input - 49]].Peff] += 1;
						cout << "\n\t\tnow you are " << skills[Curskills[input - 49]].Peff << endl;
					}
					if (skills[Curskills[input - 49]].Eeff != "NA") {
						stateE[skills[Curskills[input - 49]].Eeff] += 1;
						cout << "\n\t\tnow the enemy is " << skills[Curskills[input - 49]].Peff << endl;
					}
					sleep(5);
					system("clear");
				}
				else {
					cout << "\t\tOh! That's an invalid input. Please follow the instrucion!" << endl;
					sleep(5);
					system("clear");
					continue;
				}
			}
			else {
				cout << "\n\n\t\tYou are freezed, skip this turn" << endl;//check freezing
				stateP["Freezed"] -= 1;
				sleep(5);
			}
			if (Ian.CurHP <= 0) {//check HP
				system("clear");
				cout << "\n\n\t\tYou died..." << endl;
				sleep(5);
				return 0;
			}

			if (Ene.HP <= 0) {//check HP
				cout << "\t\tYou won this round!" << endl;
				sleep(5);
				system("clear");
				break;
			}

			if (stateE["Freezed"] == 0) {// enemy turn
				Ian.CurHP -= Ene.ATK;
				cout << "\n\t\t You are attacked by the enemy!" << endl;
				if (Ene.Peff != "NA") {
					stateP[Ene.Peff] += 1;
					cout << "\n\t\tnow you are " << Ene.Peff << endl;
				}
				if (Ene.Eeff != "NA") {
					stateE[Ene.Eeff] += 1;
					cout << "\n\t\tnow the enemy is " << Ene.Eeff << endl;
				}
			}
			else {//check freezing
				stateE["Freezed"] -= 1;
				cout << "the enemy was freezed, it can't attack you in this turn" << endl;
			}

			if (stateP["Toxicosis"] != 0) {//check state of player
				Ian.CurHP -= 5;
				cout << "\t\tYou are toxicosis, -5 HP" << endl;
				stateP["Toxicosis"] -= 1;
			}
			if (stateP["Healed"] != 0) {
				Ian.CurHP = min(Ian.CurHP + 3, Ian.MaxHP);
				cout << "\t\tYou are healed, +3 HP" << endl;
				stateP["Healed"] -= 1;
			}
			if (stateP["Enraged"] != 0) {
				Ian.ATK += 5;
				cout << "\t\tYou are Enraged, +5 ATK" << endl;
				stateP["Enraged"] -= 1;
			}
			if (stateP["Weakened"] != 0) {
				Ian.ATK -= 3;
				cout << "\t\tYou are Weakened, -3 ATK" << endl;
				stateP["Weakened"] -= 1;
			}
			if (stateP["Bleeding"] != 0) {
				Ian.CurHP -= 3;
				cout << "\t\tYou are Bleeding, -3 HP" << endl;
				stateP["Bleeding"] -= 1;
			}

			if (stateE["Toxicosis"] != 0) {//check state of enemy
				Ene.HP -= 5;
				cout << "\t\tEnemy is Bleeding, -5 HP" << endl;
				stateE["Toxicosis"] -= 1;
			}
			if (stateE["Healed"] != 0) {
				Ene.HP += 3;
				cout << "\t\tEnemy is Healed, -5 HP" << endl;
				stateE["Healed"] -= 1;
			}
			if (stateE["Enraged"] != 0) {
				Ene.ATK += 5;
				cout << "\t\tEnemy is Enraged, +5 ATK" << endl;
				stateE["Enraged"] -= 1;
			}
			if (stateE["Weakened"] != 0) {
				Ene.ATK -= 3;
				cout << "\t\tEnemy is Weakened, -3 ATK" << endl;
				stateE["Weakened"] -= 1;
			}
			if (stateP["Bleeding"] != 0) {
				Ene.HP -= 3;
				cout << "\t\tEnemy is Bleeding, -3 HP" << endl;
				stateP["Bleeding"] -= 1;
			}
			if (Ian.CurHP <= 0) {//check HP
				system("clear");
				sleep(5);
				return 0;
			}

			if (Ene.HP <= 0) {//check HP
				cout << "\t\tYou won this round!" << endl;
				sleep(5);
				system("clear");
				break;
			}
			sleep(5);
			system("clear");
		}
	}
	cout << "\t\tyou defeated the Enemies";
	delete[] ptr;
	return 1;
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
  imp();
	if (key == '3') { //3:  Exit
		return 0;
	}
	else if (key == '1') {//1: New Game
  	ReadInArchive(Ian, 1, level, CurSkills);//load the game data
	}
	else if (key == '2') {//2: Continue Game
  	ReadInArchive(Ian, 2, level, CurSkills);//load the game data
	}
	system("clear");

	for (level; level < 6; ++level) {
		Save(Ian, level, CurSkills);//save the data at the beginning of the level
		int x, y;
		x = 4, y = 4;//position of player
		int bx, by; //position of boss
		Room m[10][10];
		bool mapOpen;
		generateMap(m, level, bx, by);//generate a random map
    m[4][4].type='0';
		showMap(m, level, x, y, mapOpen);//show the map
		while (m[bx][by].cleared == 0) {
			cout << "			Press [Q] to quit the game." << endl;
			cout << "			Press [B] to modify your skills." << endl;
			cout << "			Press [W/A/S/D] to move in the map." << endl;
      FIGHT:
			char key = _getch();
			char curType;// current room type
			bool curCleared;// current room status
			curType = m[x][y].type;
			curCleared = m[x][y].cleared;
			if ((curType == 'm' || curType == 'M' || curType == 'B') && (curCleared == 0)) {//in the monster room
				if (key == 'f') {
					m[x][y].cleared = battle(Ian, m[x][y].type=='B', CurSkills);
				}
				else if (key == 'q') {//quit
					cout << "\t\t\t\tWarning!!\n\t\tYour data in this level won't be saved if you quit now!!\n\t\tPress [Q] to confirm\t\tPress [C] to cancel\n";
					key = _getch();
					system("clear");
					if (key == 'q') goto FLAG;
					if (key == 'c') {showRoom(m, x, y); goto FIGHT;}
				}
				else if (key == 'm') {//close or open the map
					if (mapOpen == true) {
						showRoom(m, x, y);
						mapOpen = false;
					}
					else {
						showMap(m, level, x, y, mapOpen);
					}
          goto FIGHT;
				}
				else {
					cout << "\t\t\tFighting is doomed!Don't run away!\n\n\n";
          sleep(4);
					m[x][y].cleared = battle(Ian, m[x][y].type=='B', CurSkills);
				}

				if (m[x][y].cleared == 0) {
					system("clear");
					cout << "\n\n\n\n\n\n\t\t\t\t\tYou are dead!\n\n";
          sleep(4);
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
          break;
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
						int code = 0;
						code = rand() % skills.size();
            itr = skills.begin();
						for (int i = 0; i < code; ++i) ++itr;
						while ((*itr).second.learned == true) {
							itr = skills.begin();
							code = rand() % skills.size();
							for (int i = 0; i <code; ++i) ++itr;
						}//roll an unlearned ability randomly
						tmpAbility = (*itr).first;
						skills[tmpAbility].learned = true;//learned
						system("clear");
						cout << "\t\t\t You have learned this skill called " << tmpAbility << endl;
						sleep(5);
						Ian.abilities.push_back(tmpAbility);//Ian learned this ability;
            int xx = 0;
            while(CurSkills[xx]!="NA"){
              xx++;
              if(xx==4)break;
            }
            if(xx<4)CurSkills[xx]=tmpAbility;//take the new skills to the current list if the number is fewer than 4
						m[x][y].cleared = 1;
						showRoom(m, x, y);
					}
					break;
				case 'b'://change skills
					changeSkills(CurSkills, Ian);
					showRoom(m, x, y);
					break;
				default:
					cout << "\t\t\tInvalid input!\n";
					sleep(3);
					showRoom(m, x, y);
				}
			}
		}
	}
  	cout<<"\n\n\n\n\t\t\tCongratulations! You defeated all the monsters!";
  	sleep(10);
	return 0;
}
