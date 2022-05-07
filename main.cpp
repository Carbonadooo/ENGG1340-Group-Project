#include<iostream>
#include<string>
#include<iomanip>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<map>
#include<algorithm>
#include<termio.h>
#include"getch.h"
#include"data.h"
#include"show.h"
#include"structures.h"
#include"battle.h"
using namespace std;

map<string, Monster> enemy; //store enemy's data
map<string, Ability> skills; //store player's ablity

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
			for (int i = 0; i < 4; i++) fin >> CurSkills[i];
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

void dead() {
	ofstream fout;
	fout.open("PlayerArchive.txt");
	fout << "dead";
	fout.close();
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
	else if (key == '2') {
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
					m[x][y].cleared = battle(Ian, m[x][y].type=='B', CurSkills, skills, enemy);
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
				}
				else {
					cout << "\t\t\tFighting is doomed!Don't run away!\n";
					m[x][y].cleared = battle(Ian, m[x][y].type=='B', CurSkills, skills, enemy);
				}

				if (m[x][y].cleared == 0) {
					system("clear");
					cout << "\n\n\n\n\n\n\t\t\t\t\tYou are dead!\n\n";
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
