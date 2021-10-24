#include "common.h"

char*  nt_file_name;
char* chr_file_name;
char* local;

EZ_Image canvas;
EZ_Font  font;


uint cursor_x, cursor_y;
uint select_x, select_y;
uint ntable_x, ntable_y;

uint active_bank;
uint active_slot;


EZ_Px NES_palette[PAL_SIZE];
uint8_t palette[4] = {0x00, 0x16, 0x37, 0x08}; //mario face palette


uint8_t clipboard[BLOCK_SIZE * NB_PLANES];
uint8_t chr_data[NB_TILES * NB_PLANES * NB_BANKS * BLOCK_SIZE];
uint8_t nametable[TABLE_SIZE];




void reload_char() {

	//read data from input file	
	FILE* fp = fopen(chr_file_name, "r");

	if (fp != NULL) {
		printf("Loading char map %s\n", chr_file_name);
		fread(chr_data, 1, NB_TILES * NB_BANKS * BLOCK_SIZE * NB_PLANES, fp);
		fclose(fp);
	}
	else 
		WARNING("Couldn't load char map %s", chr_file_name);


}


void reload_nametable() {

	//read data from input file	
	FILE* fp = fopen(nt_file_name, "r");

	if (fp != NULL) {
		printf("Loading nametable %s\n", nt_file_name);
		fread(chr_data, 1, NB_TILES * NB_BANKS * BLOCK_SIZE * NB_PLANES, fp);
		fclose(fp);
	}
	else 
		WARNING("Couldn't load nametable %s", nt_file_name);

}



void EZ_callback_init() {

	char buffer[1024];


	//load menu font and font to edit
	strcpy(buffer, local);
	strcat(buffer, "/font.pfsu");

	font = EZ_load_PSF2(buffer);
	

	//load data
	reload_char();
	reload_nametable();



	//load palette
	strcpy(buffer, local);
	strcat(buffer, "/palette.conf");

	FILE* palfile = fopen(buffer, "r");

	ASSERTM (palfile != NULL, "Couldn't load palette");

	for (int i = 0; i < PAL_SIZE; i++)	{
		int r, g, b;
		fscanf(palfile, "%d %d %d", &r, &g, &b);

		NES_palette[i].r = r;
		NES_palette[i].g = g;
		NES_palette[i].b = b;
		NES_palette[i].a = 0xff;

	}

	fclose(palfile);


	//create window
	canvas = EZ_createImage(512,512);
	EZ_bind(canvas);


}




void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeFont(font);
}