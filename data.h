// data.h
#ifndef DATA_H
#define DATA_H
#include<string>
#include<fstream>
#include"structures.h"
using namespace std;

void imp(map<string,Ability>& skills, map<string,Monster>& enemy);
void ReadInArchive(Hero& Ian, ifstream& fin, int& level, string CurSkills[4], map<string, Ability>& skills);
void Save(Hero Ian, int level, string CurSkills[4]);
#endif