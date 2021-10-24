#include "common.h"

#include <unistd.h>
#include <libgen.h>

#include <argp.h>


void EZ_callback_keyReleased(EZ_Key key) {}
void EZ_callback_mouseMoved(EZ_Mouse mouse) {}
EZ_Sample EZ_sfx_callback(double time, int channel) {return (EZ_Sample)0;}



int parse_opt (int key, char *arg, struct argp_state *state) { 
	
	switch (key) { 
		case 'c': chr_file_name = arg; break; 
		case 'n': nt_file_name = arg; break;
		case 'b': nb_banks = atoi(arg); break; 

	} 
	return 0; 
}


struct argp_option options[] = { 
		{ "char", 		'c', "FILENAME", 0, "Char file name", 	    0}, 
		{ "nametable",  'n', "FILENAME", 0, "Nametable file name",  0}, 
		{ "banks", 		'b', "NUM",      0, "Number of char banks", 0},
		{0}
	};

char doc[] = "NES char map and nametable editor.";
char args_doc[] = "";

struct argp argp = { options, parse_opt, args_doc, doc }; 



int main (int argc, char **argv) {

	char buffer[PATH_MAX];

	//!\\ only works on linux
	readlink("/proc/self/exe", buffer, PATH_MAX);
	dirname(buffer);
	local = buffer;

	argp_parse (&argp, argc, argv, 0, 0, 0); 


	EZ_start();
	EZ_join();

	return 0;
}
