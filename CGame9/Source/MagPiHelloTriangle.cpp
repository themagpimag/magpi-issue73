/*
Simple example of a main function creating a game class and updating it
*/

#include "Game.h"
#include <termios.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <iostream>
#include <streambuf>
#include <stdlib.h>
#include <string>
#include <sstream>

// we need to save the original terminal io settings
struct termio original_termio;
struct termio new_termio;
int main(int argc, char *argv[])
{

	
// remember this? this lets us redirect our OS keyboard systems 
	
	int fd = fileno(stdin);
	ioctl(fd, TCGETA, &original_termio);

	new_termio = original_termio;
	new_termio.c_cc[VMIN] = 0;
	new_termio.c_cc[VTIME] = 0;
	new_termio.c_lflag = 0; // stop echo (and other things)
	
	ioctl(fd, TCSETA, &new_termio);
	
	

	Game TheGame; // create an instance of  game.
	
	TheGame.Update();
	
// and this puts them back when done
	fd = fileno(stdin);
	ioctl(fd, TCSETA, &original_termio);  // put the old one back
	
} 
