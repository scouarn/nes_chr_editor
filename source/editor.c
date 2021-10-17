#include "ezGfx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define NB_TILES 512
#define NB_PLANES  2 
#define BLOCK_SIZE 8
#define MAP_SIZE  16
#define EDIT_RES  16
#define PAL_SIZE  64
#define PAL_RES    8

char fname[256];

EZ_Image canvas;
EZ_Font  font;


uint cursor_x, cursor_y;
uint select_x, select_y;

uint edit_char = 0;

EZ_Px NES_palette[PAL_SIZE];
uint8_t palette[4] = {0x00, 0x16, 0x37, 0x08}; //mario face palette
uint active_slot = 1;

uint8_t clipboard[BLOCK_SIZE*NB_PLANES];
uint8_t chr_data[NB_TILES*BLOCK_SIZE*NB_PLANES];



void draw_block(unsigned int id, int x0, int y0, int res) {


	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	for (int y = 0; y < BLOCK_SIZE; y++)
	for (int x = 0; x < BLOCK_SIZE; x++) {

		//take to two bits corresponding to the color information
		unsigned char col_id = 0;
		col_id |= (  plane1[y] >> (BLOCK_SIZE - x - 1)) & 0x1;
		col_id |= (( plane2[y] >> (BLOCK_SIZE - x - 1)) & 0x1) << 1;

		EZ_Px col = NES_palette[palette[col_id]];

		EZ_draw2D_fillRect(canvas, col, (x0+x) * res, (y0+y) * res, res, res);

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
	FILE* charfile = fopen(fname, "r");

	if (charfile != NULL) {
		fread(chr_data, 1, NB_TILES * BLOCK_SIZE * NB_PLANES, charfile);
		fclose(charfile);
	}


}

void EZ_callback_init() {

	//load menu font and font to edit
	font = EZ_load_PSF2("font.pfsu");
	

	//load chr data
	reload_data();



	//load palette
	FILE* palfile = fopen("palette.conf", "r");

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
	canvas = EZ_createImage(256,256);
	EZ_bind(canvas);


}


void EZ_callback_draw(double dt) {

	//clear
	EZ_draw2D_clear(canvas, EZ_BLUE);
	EZ_draw2D_setTranslate(0, 0);


	//edit char
	draw_block(edit_char, 0, 0, EDIT_RES);


	//char map
	for (int x = 0; x < MAP_SIZE; x++)
	for (int y = 0; y < MAP_SIZE; y++) {

		draw_block(x + y * MAP_SIZE, x*BLOCK_SIZE, (y + EDIT_RES)*BLOCK_SIZE, 1);

	}


	//cursor 
	EZ_draw2D_rect(canvas, EZ_WHITE, cursor_x*EDIT_RES, cursor_y*EDIT_RES, EDIT_RES, EDIT_RES);

	//select
	EZ_draw2D_rect(canvas, EZ_WHITE, select_x*BLOCK_SIZE, BLOCK_SIZE*EDIT_RES + select_y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);

	//borders
	EZ_draw2D_rect(canvas, EZ_WHITE, 0, 0, BLOCK_SIZE*EDIT_RES, BLOCK_SIZE*EDIT_RES + 1);
	EZ_draw2D_rect(canvas, EZ_WHITE, 0, BLOCK_SIZE*EDIT_RES, BLOCK_SIZE*MAP_SIZE, BLOCK_SIZE*MAP_SIZE);


	//whole palette
	for (int i = 0; i < 16; i++)
	for (int j = 0; j <  4; j++)
		EZ_draw2D_fillRect(
			canvas, NES_palette[i+j*16], 
			BLOCK_SIZE*EDIT_RES + i*PAL_RES, j*PAL_RES, 
			PAL_RES, PAL_RES
		);

	//current palette
	char buffer[4];
	for (int i = 0; i < 4; i++) {

		//color square
		EZ_draw2D_fillRect(canvas, NES_palette[palette[i]],
			BLOCK_SIZE*EDIT_RES + i*PAL_RES*4, PAL_RES*4,
			PAL_RES*4, PAL_RES*4 

		);

		//color id
		sprintf(buffer, "%02X", palette[i]);
		EZ_draw2D_printStr(
			canvas, buffer, font, EZ_WHITE, EZ_BLUE, 
			BLOCK_SIZE*EDIT_RES + i*PAL_RES*4, PAL_RES*4,
			2, 1
		);


	}

	//active color slot (cursor)
	EZ_draw2D_rect(canvas, EZ_WHITE, 
		BLOCK_SIZE*EDIT_RES + active_slot*PAL_RES*4, PAL_RES*4,
		PAL_RES*4, PAL_RES*4 
	);


	
	
}



void onclick() {
	EZ_Mouse m = EZ_getMouse();

	uint x = m.x;
	uint y = m.y;

	//top left
	if (x < BLOCK_SIZE * EDIT_RES && y < BLOCK_SIZE * EDIT_RES) {
		cursor_x = x / EDIT_RES;
		cursor_y = y / EDIT_RES;
	}

	//bottom left
	else if (x < BLOCK_SIZE * EDIT_RES && y > BLOCK_SIZE * EDIT_RES && y < canvas.h) {
		select_x = x / BLOCK_SIZE;
		select_y = (y - BLOCK_SIZE * EDIT_RES) / BLOCK_SIZE;
		edit_char = select_x + select_y*MAP_SIZE; 
	}

	//top right
	else if (x > BLOCK_SIZE * EDIT_RES && y < BLOCK_SIZE * EDIT_RES) {

		if (y < PAL_RES * 4) {
			int px = (x - BLOCK_SIZE * EDIT_RES) / PAL_RES;
			int py = y / PAL_RES;
			palette[active_slot] = (px + py*16) % PAL_SIZE;
		}
		else if (y < PAL_RES * 8) {
			active_slot = (x - BLOCK_SIZE * EDIT_RES) / (PAL_RES * 4);
			active_slot %= 4;
		}
	}


}



void EZ_callback_keyPressed(EZ_Key key) {

	switch (key.keyCode) {

	//select
	case K_LMB :
		onclick();
	break;

	//move cursor
	case K_LEFT  : cursor_x--; cursor_x %= BLOCK_SIZE; break;
	case K_UP    : cursor_y--; cursor_y %= BLOCK_SIZE; break;
	case K_RIGHT : cursor_x++; cursor_x %= BLOCK_SIZE; break;
	case K_DOWN  : cursor_y++; cursor_y %= BLOCK_SIZE; break;
		

	//move character selector
	case KP_4 : select_x--; select_x %= MAP_SIZE; break;
	case KP_8 : select_y--; select_y %= MAP_SIZE; break;
	case KP_6 :	select_x++; select_x %= MAP_SIZE; break;
	case KP_2 : select_y++; select_y %= MAP_SIZE; break;

	//update selector
	case K_RETURN :
	 	edit_char = select_x + select_y*MAP_SIZE; 
 	break;


 	//active color slot
 	case K_PGUP :
 		active_slot = (active_slot + 1) % 4;
 	break;

 	case K_PGDN :
 		active_slot = (active_slot - 1) % 4;
 	break;


	//draw
	case K_SPACE :
		set_pixel(edit_char, cursor_x, cursor_y, active_slot);
	break;

	//erase
	case K_X :
		set_pixel(edit_char, cursor_x, cursor_y, 0);
	break;


	//exit
	case K_ESCAPE : EZ_stop(); break; //exit



	//copy
	case K_C :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			uint selected = select_x + select_y*MAP_SIZE;

			clipboard[i] = chr_data[i + selected * BLOCK_SIZE * NB_PLANES]; 
		}

	break;

	//paste
	case K_V :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			uint selected = select_x + select_y*MAP_SIZE;

			chr_data[i + selected * BLOCK_SIZE * NB_PLANES] = clipboard[i]; 
		}
	break;


	//reload
	case K_R :
		printf("Reloading %s\n", fname);
		reload_data();
	break;

	//save
	case K_S :
		printf("Saving %s\n", fname);
		
		FILE* charfile = fopen(fname, "w");

		if (charfile != NULL) {
			fwrite(chr_data, 1, NB_TILES * BLOCK_SIZE * NB_PLANES, charfile);
			fclose(charfile);
		}
		else 
			WARNING("Couldn't write to file %s", fname);

	break;

	//screenshot
	case K_TAB : 
		printf("Saving screenshot\n");
		EZ_save_BMP(canvas, "./screenshot.bmp");
	break;

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

