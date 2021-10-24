#include "common.h"


void EZ_callback_keyReleased(EZ_Key key) {}
void EZ_callback_mouseMoved(EZ_Mouse mouse) {}
EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}




const char* usage = "USAGE : chredit filename\n"
					"If the file doesn't existe,"
					"it wille be created.\n"
;



int main (int argc, char **argv) {

	char buffer[PATH_MAX];

	//!\\ only works on linux
	readlink("/proc/self/exe", buffer, PATH_MAX);
	
	dirname(buffer);
	local = buffer;


	if(argc > 1)
		chr_file_name = argv[1];

	else {
		printf(usage);
		exit(1);
	}

	EZ_start();
	EZ_join();


	return 0;
}
