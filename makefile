getch.o: getch.cpp getch.h
	g++ -c getch.cpp

battle.o: battle.cpp battle.h structures.h getch.h
	g++ -c battle.cpp

data.o: data.cpp data.h structures.h getch.h
	g++ -c data.cpp

show.o: show.cpp show.h structures.h
	g++ -c show.cpp

main.o: main.cpp battle.h structures.h data.h show.h getch.h
	g++ -c main.cpp

main: main.o battle.o show.o data.o getch.o
	g++ main.o battle.o show.o data.o getch.o -o main


