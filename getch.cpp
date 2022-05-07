#include"getch.h"
#include<iostream>
#include<unistd.h>
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
