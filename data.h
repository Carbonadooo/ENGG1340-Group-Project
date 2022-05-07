// data.h
#ifndef DATA_H
#define DATA_H
#include<string>
#include<fstream>
#include"structures.h"
using namespace std;

void ReadInArchive(Hero& Ian, int x, int& level, string CurSkills[4]);
void Save(Hero Ian, int level, string CurSkills[4]);
#endif
