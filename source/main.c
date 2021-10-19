#include "ezGfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char fname[];

const char* usage = "USAGE : chredit filename\n"
					"If the file doesn't existe,"
					"it wille be created.\n"
;


int main (int argc, char **argv) {

	//copy arg
	if(argc > 1)
		strcpy(fname, argv[1]);

	else {
		printf(usage);
		exit(1);
	}

	EZ_start();
	EZ_join();


	return 0;
}
