#include<iostream>
#include<map>
#include"data.h"



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
