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
				txt = "3: Exit";
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


struct Ability {
	int damage, heal, mana, number; // mana: the cost of the skills
	string Peff, Eeff; // the skills that will have some effect on Player or Enemy (Player's effect and Enemy's effect)
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
	string ability="", monster="";
};


map<string, Monster> enemy; //store enemy's data
map<string, Ability> skills; //store player's ablity

class Hero {
public:
	int MaxHP,CurHP,ATK;
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

void ReadInAchive(Hero& Ian, ifstream& fin,int& level) {
	Ability tempSkill;
	string command;
	while (fin >> command) {
		if (command == "Level")  fin >> level;
		if (command == "MaxHP") fin >> Ian.MaxHP;
		if (command == "CurHP") fin >> Ian.CurHP;
		if (command == "Attack") fin >> Ian.ATK;
		if (command == "Ability")
			while (fin >> command)
				Ian.abilities.push_back(command);
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

void Save(Hero Ian, int level) {
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
	fout.close();
}

void generateMap(Room m[10][10], int level,int &bx, int &by) {
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

void showMap(Room m[10][10], int level, int cx, int cy, bool &mapOpen) {
	mapOpen = true;
	system("cls");
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
	system("cls");
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
			txt[12] = "Press [E] to learn this skill!";
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


/* form of selected skills: only store the name. PP: only store the numbers remained 
can use this to import/reefresh the PP when learned a new skill or get to the revive room:
	for (int i=0; i < 4; i++){
        	PP[i]=skills[Curski[i]].mana;
    	}// import/refresh the mana(how many times can be used)*/
// input: Hero data, have boss or not, selected skills, the PP of skills
int battle(Hero &Ian,bool boss,string Curskills[4], int PP[4]){ //return 1: player died, return 0: player won
    srand(time(NULL));
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
    int enemyNum = rand()%3+1,enemyCode; //generate 1-3 random enemies to fight
    string *ptr;
    if (boss == 1){//store the generate enemies
        ptr = new string[enemyNum+1];
        } 
    else{
        ptr = new string[enemyNum];// if have boss, add the boss at the end of the enemyList
    }
    map<string, Monster>::iterator itr;
    for (int i=0; i < enemyNum; i++){//generating
        itr = enemy.begin();
        enemyCode = rand()%11;
        if (enemyCode == 9){
            enemyCode = 12;
        }
        else if (enemyCode == 6){
            enemyCode = 11;
        }
        cout << "the code is " << enemyCode << endl;
        for (int j=0; j<enemyCode; j++){itr++;}
        cout << "enemy is " << (*itr).first << endl;
        ptr[i]=(*itr).first;
    }
    if (boss == 1){// add the boss
        enemyCode = rand()%2;
        if (enemyCode == 0){ptr[enemyNum]="PitLord";}
        else{ptr[enemyNum]="VoidTerror";} 
    }
    char input;


    
    for(int i=0; i<(enemyNum+boss); i++){//fight!!!
        cout << "Now you are fight with " << ptr[i] << endl;
        Monster Ene = enemy[ptr[i]];
        while (Ene.HP >0 && Ian.HP>0){//leave the while loop when palyer beat an enemy or player die
            if (PP[0]==0 && PP[0]==0 && PP[0]==0 && PP[0]==0){// if running out all pp, lose the game
                cout << "You lose the battle because all the PP are used up" << endl;
                return 1;
            }
            if (stateP["Freezed"] == 0){//check freeze
                cin >> input;// get valid input
                while (input != '1' && input != '2' && input != '3' && input != '4'){
                    cout << "invalid input" << endl;
                    cin >> input;
                }
                if (PP[input-49]>0){// player turn
                    cout << "You used the " << Curskills[input-49]  << endl;
                    Ene.HP -= (skills[Curskills[input-49]].damage+Ian.ATK);
                    PP[input-49]-=1;
                    if (skills[Curskills[input-49]].Peff != "NA"){
                    stateP[skills[Curskills[input-49]].Peff]+=1;
                    }
                    if (skills[Curskills[input-49]].Eeff != "NA"){
                        stateE[skills[Curskills[input-49]].Eeff]+=1;
                    }
                }
                else {
                    cout << "Oh! the PP of the skill has been used up! choose another skill" << endl;
                    continue;
                }
            }
            else{
                cout << "You are freezed, skip this turn" << endl;//check freezing
                stateP["Freezed"] -= 1;
            }
            if (Ene.HP<=0){//check HP
                    cout << "You won in this battle" << endl;
                    break;
            }
            else if (Ian.HP<=0){//check HP
                cout << "You died" << endl;
                return 1;
            }
            if (stateE["Freezed"] == 0){// enemy turn
                Ian.HP -= Ene.ATK;
                cout << " You are attacked by the enemy!" << endl;
                cout << " testing HPs " << Ene.HP << Ian.HP << endl;
                if (Ene.Peff != "NA"){
                    stateP[Ene.Peff]+=1;
                }
                if (Ene.Eeff != "NA"){
                    stateE[Ene.Eeff]+=1;
                }
            }
            else{//check freezing
                stateE["Freezed"]-=1;
                cout << "the enemy was freezed, it can't attack you in this turn";
            }

            if (stateP["Toxicosis"] != 0){//check state of player
                Ian.HP -= 5;
                cout << "You are toxicosis, -5 HP" << endl;
                stateP["Toxicosis"]-=1;
            }
            if (stateP["Healed"] != 0){
                Ian.HP += 3;
                cout << "You are healed, +3 HP" << endl;
                stateP["Healed"]-=1;
            }
            if (stateP["Enraged"] != 0){
                Ian.ATK += 5;
                stateP["Enraged"]-=1;
            }
            if (stateP["Weakened"] != 0){
                Ian.ATK -= 3;
                stateP["Weakened"]-=1;
            }
            if (stateP["Bleeding"] != 0){
                Ian.HP -= 3;
                stateP["Bleeding"]-=1;
            }

            if (stateE["Toxicosis"] != 0){//check state of enemy
                Ene.HP -= 5;
                stateE["Toxicosis"]-=1;
            }
            if (stateE["Healed"] != 0){
                Ene.HP += 3;
                stateE["Healed"]-=1;
            }
            if (stateE["Enraged"] != 0){
                Ene.ATK += 5;
                stateE["Enraged"]-=1;
            }
            if (stateE["Weakened"] != 0){
                Ene.ATK -= 3;
                stateE["Weakened"]-=1;
            }
            if (stateP["Bleeding"] != 0){
                Ene.HP -= 3;
                stateP["Bleeding"]-=1;
            }
            if (Ene.HP<=0){//check HP
                    cout << "You won in this battle" << endl;
                    break;
            }
            else if (Ian.HP<=0){//check HP
                cout << "You died" << endl;
                return 1;
            }
        }   
    }
    cout << "you defeated the Enemies";
    delete [] ptr;
    return 0;
}

void initialize (Hero &Ian){
	Ian.MAXHP = 100;
	Ian.CurHP = 100;
	abilities.push_back("Fireblast");
	Ian.ATK=0;
}



int main() {
	FLAG:
	srand(time(NULL));
	Hero Ian;//The Hero of the game
	initialize(Ian);
	frontPage();//show the front page
	char key;
	key = GetOption();//Get first option
	int level = 1;
	ifstream fin;
	if (key == '3') { //3:  Exit
		return 0;
	}
	else if (key == '1') {//1: New Game
		fin.open("DefaultArchive.txt");//read the default archive	
	}
	else if (key == '2') {//2: Continue Game
		fin.open("PlayerArchive.txt");//read the player archive
	}
	system("cls");
	imp();
	ReadInAchive(Ian,fin,level);//load the game data
	fin.close();

	for (level; level < 6; ++level) {
		Save(Ian,level);//save the data at the beginning of the level
		int x, y;
		x = 4, y = 4;//position of player
		int bx, by; //position of boss
		Room m[10][10];
		bool mapOpen;
		generateMap(m, level,bx,by);//generate a random map
		showMap(m,level,x,y,mapOpen);//show the map
		while (m[bx][by].cleared == 0) {
			cout << "			Press [Q] to quit the game." << endl;
			cout << "			Press [B] to modify your skills." << endl;
			cout << "			Press [W/A/S/D] to move in the map." << endl;
			char key = _getch();
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
				system("cls");
				if (key == 'q') return 0;
				if (key == 'c') goto FLAG;

			case 'w':
			case 's':
			case 'a':
			case 'd':


			case 'e':
			case 'f':
			case 'b'://change skills
			}
		}
	}
	return 0;
}
