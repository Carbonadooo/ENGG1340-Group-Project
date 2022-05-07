#include <string>
#include <iostream>
#include <map>
#include <vector>
#include "structures.h"


// battle.h
#ifndef BATTLE_H 
#define BATTLE_H 
bool battle(Hero& Ian, bool boss, string Curskills[4],map<string,Ability> skills,map<string,Monster> enemy);
#endif
