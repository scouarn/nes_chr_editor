#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>


#define NB_TILES 256
#define NB_PLANES  2
#define NB_BANKS   2 
#define BLOCK_SIZE 8

#define EDIT_RES  32

#define MAP_SIZE  16
#define MAP_RES    2

#define PAL_SIZE  64
#define PAL_RES   16

#define TABLE_W   32
#define TABLE_H   30
#define TABLE_SIZE 1024
#define TABLE_RES  1

#define BORDER_COLOR EZ_WHITE
#define CURSOR_COLOR EZ_YELLOW

char*  nt_file_name;
char* chr_file_name;

char* local;

EZ_Image canvas;
EZ_Font  font;


uint cursor_x = 0, cursor_y = 0;
uint select_x = 0, select_y = 0;
uint ntable_x = 0, ntable_y = 0;

uint active_bank = 0;
uint active_slot = 0;

#define EDIT_CHAR (NB_TILES*active_bank + select_x + select_y*MAP_SIZE)


EZ_Px NES_palette[PAL_SIZE];
uint8_t palette[4] = {0x00, 0x16, 0x37, 0x08}; //mario face palette


uint8_t clipboard[BLOCK_SIZE * NB_PLANES];
uint8_t chr_data[NB_TILES * NB_PLANES * NB_BANKS * BLOCK_SIZE];
uint8_t nametable[TABLE_SIZE];



void draw_block(int id, int x0, int y0, int res, bool hflip, bool vflip) {


	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	for (int y = 0; y < BLOCK_SIZE; y++)
	for (int x = 0; x < BLOCK_SIZE; x++) {

		//take to two bits corresponding to the color information
		unsigned char col_id = 0;

		col_id |= (  plane1[y] >> x) & 0x1;
		col_id |= (( plane2[y] >> x) & 0x1) << 1;

		EZ_Px col = NES_palette[palette[col_id]];

		int xf =  hflip ? x : BLOCK_SIZE - x - 1;
		int yf = !vflip ? y : BLOCK_SIZE - y - 1;


		EZ_draw2D_fillRect(canvas, col, x0 + (xf) * res, y0 + (yf) * res, res, res);

	}

}


void set_pixel(unsigned int id, int x, int y, int col) {
	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	int offset = BLOCK_SIZE - x - 1;

	plane1[y] &= ~(1 << offset);
	plane2[y] &= ~(1 << offset);

	plane1[y] |= (col  & 1) << offset;
	plane2[y] |= (col >> 1) << offset;

}



void reload_data() {

	//read data from input file	
	FILE* charfile = fopen(chr_file_name, "r");

	if (charfile != NULL) {
		fread(chr_data, 1, NB_TILES * NB_BANKS * BLOCK_SIZE * NB_PLANES, charfile);
		fclose(charfile);
	}


}

void EZ_callback_init() {

	char buffer[1024];


	//load menu font and font to edit
	strcpy(buffer, local);
	strcat(buffer, "/font.pfsu");

	font = EZ_load_PSF2(buffer);
	

	//load chr data
	reload_data();



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


void EZ_callback_draw(double dt) {

	//clear
	EZ_draw2D_clear(canvas, EZ_BLUE);
	EZ_draw2D_setTranslate(0, 0);




	//edit char
	draw_block(EDIT_CHAR, 0, 0, EDIT_RES, false, false);


	//char map
	for (int x = 0; x < MAP_SIZE; x++)
	for (int y = 0; y < MAP_SIZE; y++) {

		draw_block(active_bank*NB_TILES + x + y*MAP_SIZE, 
				   x*BLOCK_SIZE*MAP_RES, 
				   canvas.h/2 + y*BLOCK_SIZE*MAP_RES, 
				   MAP_RES, 
				   false, 
				   false
				);

	}

	//nametable
	for (int x = 0; x < TABLE_W; x++)
	for (int y = 0; y < TABLE_H; y++) {

		draw_block(NB_TILES*active_bank + nametable[x + y*TABLE_W], 
				   canvas.w/2 + x*BLOCK_SIZE*TABLE_RES, 
				   canvas.h/2 + y*BLOCK_SIZE*TABLE_RES, 
				   1, 
				   false, 
				   false
				);

	}



	//color palette
	for (int i = 0; i < 16; i++)
	for (int j = 0; j <  4; j++)
		EZ_draw2D_fillRect(
			canvas, NES_palette[i+j*16], 
			canvas.w/2 + i*PAL_RES, 
			j*PAL_RES, 
			PAL_RES, PAL_RES
		);



	//active slot color on the main palette
	{
	int col = palette[active_slot];

	int x = col % 16;
	int y = col / 16;


	//current 4 color palette (with IDs)
	for (int i = 0; i < 4; i++) {

		//color square
		EZ_draw2D_fillRect(canvas, NES_palette[palette[i]],
			canvas.w/2 + i*PAL_RES*4, PAL_RES*4,
			PAL_RES*4, PAL_RES*4 

		);

		//color id
		char buffer[4];
		sprintf(buffer, "%02X", palette[i]);
		EZ_draw2D_printStr(
			canvas, buffer, font, EZ_WHITE, EZ_BLUE, 
			canvas.w/2 + i*PAL_RES*4, PAL_RES*4,
			2, 1
		);

	}


	//borders
	EZ_draw2D_line(canvas, BORDER_COLOR, 0, canvas.h/2, canvas.w, canvas.h/2);
	EZ_draw2D_line(canvas, BORDER_COLOR, canvas.w/2, 0, canvas.w/2, canvas.h);



	//cursor 
	EZ_draw2D_rect(canvas, CURSOR_COLOR, 
		cursor_x*EDIT_RES, 
		cursor_y*EDIT_RES, 
		EDIT_RES, EDIT_RES
	);

	//select
	EZ_draw2D_rect(canvas, CURSOR_COLOR,
		select_x*BLOCK_SIZE*MAP_RES, 
		canvas.h/2 + select_y*BLOCK_SIZE*MAP_RES, 
		BLOCK_SIZE*MAP_RES, BLOCK_SIZE*MAP_RES
	);

	//name table cursor
	EZ_draw2D_rect(canvas, CURSOR_COLOR, 
		canvas.w/2 + ntable_x*BLOCK_SIZE, 
		canvas.h/2 + ntable_y*BLOCK_SIZE, 
		BLOCK_SIZE, BLOCK_SIZE
	);

	//active color slot (cursor)
	EZ_draw2D_rect(canvas, CURSOR_COLOR, 
		canvas.w/2 + active_slot*PAL_RES*4, 
		PAL_RES*4,
		PAL_RES*4, PAL_RES*4 
	);


	//color select cursor
	EZ_draw2D_rect(
			canvas, CURSOR_COLOR, 
			canvas.w/2 + x*PAL_RES, 
			y*PAL_RES, 
			PAL_RES, PAL_RES
		);
	}


	//informations
	char buffer[64];

	sprintf(buffer, "Editing $%02X\nTile at %d, %d\n", EDIT_CHAR, ntable_x, ntable_y);

	EZ_draw2D_printStr(canvas, buffer, font, 
			BORDER_COLOR, EZ_BLUE, 
			canvas.w/2 + font.w_px, 
			canvas.h/4,
			24, 16
	);


}



void onclick() {
	EZ_Mouse m = EZ_getMouse();

	uint x = m.x;
	uint y = m.y;

	//top left
	if (x < canvas.w/2 && y < canvas.w/2) {
		cursor_x = x / EDIT_RES;
		cursor_y = y / EDIT_RES;
	}

	//bottom left
	else if (x < canvas.w/2 && y > canvas.h/2 && y < canvas.h) {
		select_x = x / (BLOCK_SIZE * MAP_RES);
		select_y = (y - canvas.h/2) / (BLOCK_SIZE * MAP_RES);
	}

	//bottom right
	else if (x > canvas.w/2 && x < canvas.w && y > canvas.h/2 && y < canvas.h) {
		ntable_x = (x - canvas.w/2) / (BLOCK_SIZE * TABLE_RES);
		ntable_y = (y - canvas.h/2) / (BLOCK_SIZE * TABLE_RES);
	}

	//top right
	else if (x > canvas.w/2 && x < canvas.w && y < canvas.h/2) {

		if (y < PAL_RES * 4) {
			int px = (x - canvas.w/2) / PAL_RES;
			int py = y / PAL_RES;
			palette[active_slot] = (px + py*16) % PAL_SIZE;
		}
		else if (y < PAL_RES * 8) {
			active_slot = (x - canvas.w/2) / (PAL_RES * 4);
			active_slot %= 4;
		}
	}


}


void EZ_callback_keyPressed(EZ_Key key) {

	bool ctrl  = EZ_getKey(K_LCTRL).held  || EZ_getKey(K_RCTRL).held;
	bool alt   = EZ_getKey(K_LALT).held   || EZ_getKey(K_RALT).held;
	bool shift = EZ_getKey(K_LSHIFT).held || EZ_getKey(K_RSHIFT).held;

	int inc = ctrl*3 + 1;

	switch (key.keyCode) {


	//mouse
	case K_LMB :
		onclick();
	break;


	//move cursor
	case K_LEFT  : 
		if    (shift) {select_x -= inc; select_x %= MAP_SIZE;}
		else if (alt) {ntable_x -= inc; ntable_x %= TABLE_W;}
		else 	 	  {cursor_x -= inc; cursor_x %= BLOCK_SIZE;} 
	break;

	case K_UP    :
		if 	  (shift) {select_y -= inc; select_y %= MAP_SIZE;}
		else if (alt) {ntable_y -= inc; ntable_y %= TABLE_H;}
		else 	      {cursor_y -= inc; cursor_y %= BLOCK_SIZE;}
	break;

	case K_RIGHT : 
		if    (shift) {select_x += inc; select_x %= MAP_SIZE;}
		else if (alt) {ntable_x += inc; ntable_x %= TABLE_W;}
		else 	      {cursor_x += inc; cursor_x %= BLOCK_SIZE;}
	break;

	case K_DOWN  : 
		if    (shift) {select_y += inc; select_y %= MAP_SIZE;}
		else if (alt) {ntable_y += inc; ntable_y %= TABLE_H;}
		else 	 	  {cursor_y += inc; cursor_y %= BLOCK_SIZE;} 
	break;



 	//active color slot
 	case K_OPEN :
 		active_slot -= inc; 
 		active_slot %= 4;
 	break;

 	case K_CLOSE :
 		active_slot += inc; 
 		active_slot %= 4;
 	break;


	//draw
	case K_1 ... K_4 :
		set_pixel(EDIT_CHAR, cursor_x, cursor_y, key.keyCode - K_1);
	break;


 	//change color
 	case K_PLUS :
 		palette[active_slot] += inc;
		palette[active_slot] %= PAL_SIZE;
 	break;

 	case K_MINUS :
		palette[active_slot] -= inc;
		palette[active_slot] %= PAL_SIZE; 	
 	break;

	case K_PGDN :
		active_bank += inc;
		active_bank %= NB_BANKS;
	break;

	case K_PGUP :
		active_bank += inc;
		active_bank %= NB_BANKS;
	break;



	//set tile to edit char
	case K_RETURN :
		nametable[ntable_x + ntable_y*TABLE_W] = select_x + select_y*MAP_SIZE;
	break;

	//set tile to 0
	case K_DEL :
		nametable[ntable_x + ntable_y*TABLE_W] = 0;
	break;


	//flip horizontally
	case K_F :

	break;


	//flip vertically
	case K_G :

	break;


	//point
	case K_P : {
		int i = nametable[ntable_x + ntable_y*TABLE_W];
		select_x = i % MAP_SIZE;
		select_y = i / MAP_SIZE;
	}
	break;



	//copy
	case K_C :
	
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			clipboard[i] = chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]; 
		}

	break;


	//paste
	case K_V :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = clipboard[i]; 
		}
	break;



	//reload
	case K_Z :

		if (ctrl) {
			printf("Reloading %s\n", chr_file_name);
			reload_data();
		}	

	break;


	//save
	case K_S :

		if (ctrl) {
			printf("Saving %s\n", chr_file_name);
			
			FILE* charfile = fopen(chr_file_name, "w");

			if (charfile != NULL) {
				fwrite(chr_data, 1, NB_TILES * NB_BANKS * BLOCK_SIZE * NB_PLANES, charfile);
				fclose(charfile);
			}
			else 
				WARNING("Couldn't write to file %s", chr_file_name);
		}


	break;


	//screenshot
	case K_TAB :
		printf("Saving screenshot\n");
		EZ_save_BMP(canvas, "./screenshot.bmp");
	break;


	//exit
	case K_ESCAPE : EZ_stop(); break; //exit


	default : 
		break;
	}


}

void EZ_callback_keyReleased(EZ_Key key) {}

void EZ_callback_mouseMoved(EZ_Mouse mouse) {}


void EZ_callback_kill() {
  EZ_freeImage(canvas);
  EZ_freeFont(font);
}

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