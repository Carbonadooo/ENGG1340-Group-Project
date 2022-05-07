#include<iostream>
#include<map>
#include<unistd.h>
#include"data.h"


void Save(Hero Ian, int level, string CurSkills[4]) {
	ofstream fout;
	fout.open("PlayerArchive.txt");
	fout << "alive"  << endl;
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
