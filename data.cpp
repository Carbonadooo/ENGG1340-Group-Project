#include<iostream>
#include<map>
#include"data.h"

void imp(map<string,Ability>& skills, map<string,Monster>& enemy) {//import data
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

void ReadInArchive(Hero& Ian, ifstream& fin, int& level, string CurSkills[4], map<string, Ability>& skills) {
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