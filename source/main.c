#include "ezGfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char fname[];

int main (int argc, char **argv) {

	//copy arg
	if(argc > 1)
		strcpy(fname, argv[1]);


	EZ_start();
	EZ_join();


	return 0;
}
