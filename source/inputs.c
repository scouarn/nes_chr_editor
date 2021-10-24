#include "common.h"


void set_pixel(unsigned int id, int x, int y, int col) {
	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	int offset = BLOCK_SIZE - x - 1;

	plane1[y] &= ~(1 << offset);
	plane2[y] &= ~(1 << offset);

	plane1[y] |= (col  & 1) << offset;
	plane2[y] |= (col >> 1) << offset;

}



void onclick() {
	EZ_Mouse m = EZ_getMouse();

	uint x = m.x;
	uint y = m.y;

	//top left -- select pixel
	if (x < canvas.w/2 && y < canvas.w/2) {
		cursor_x = x / EDIT_RES;
		cursor_y = y / EDIT_RES;
	}

	//bottom left -- select char
	else if (x < canvas.w/2 && y > canvas.h/2 && y < canvas.h) {
		select_x = x / (BLOCK_SIZE * MAP_RES);
		select_y = (y - canvas.h/2) / (BLOCK_SIZE * MAP_RES);
	}

	//bottom right -- select nametable tile
	else if (x > canvas.w/2 && x < canvas.w && y > canvas.h/2 && y < canvas.h) {
		ntable_x = (x - canvas.w/2) / (BLOCK_SIZE * TABLE_RES);
		ntable_y = (y - canvas.h/2) / (BLOCK_SIZE * TABLE_RES);
	}

	//top right
	else if (x > canvas.w/2 && x < canvas.w && y < canvas.h/2) {

		//pick color
		if (y < PAL_RES * 4) {
			int px = (x - canvas.w/2) / PAL_RES;
			int py = y / PAL_RES;
			palette[active_pal][active_slot] = (px + py*16) % PAL_SIZE;
		}

		//select active color & palette
		else if (x > canvas.w/4*3) {
			active_slot = ((x - canvas.w/4*3) / (PAL_RES * 2)) % 4;
			active_pal  = ((y - PAL_RES*4)    / (PAL_RES * 2)) % NB_PALS;
		}
	}


}


#define CTRL_MASK   (1 << 31)
#define ALT_MASK    (1 << 30)
#define SHIFT_MASK  (1 << 29)

void EZ_callback_keyPressed(EZ_Key key) {

	bool ctrl  = EZ_getKey(K_LCTRL).held  || EZ_getKey(K_RCTRL).held;
	bool alt   = EZ_getKey(K_LALT).held   || EZ_getKey(K_RALT).held;
	bool shift = EZ_getKey(K_LSHIFT).held || EZ_getKey(K_RSHIFT).held;

	int inc = ctrl*3 + 1;

	switch (key.keyCode | alt<<30 | shift<<29) {


	//mouse
	case K_LMB : onclick(); break;


	//select cursor
	case SHIFT_MASK | K_LEFT  : select_x = (select_x - inc) % MAP_SIZE; break;
	case SHIFT_MASK | K_UP    : select_y = (select_y - inc) % MAP_SIZE; break;
	case SHIFT_MASK | K_RIGHT : select_x = (select_x + inc) % MAP_SIZE; break;
	case SHIFT_MASK | K_DOWN  : select_y = (select_y + inc) % MAP_SIZE; break;

	//name table cursor
	case ALT_MASK | K_LEFT  : ntable_x = (ntable_x - inc) % TABLE_W; break;
	case ALT_MASK | K_UP    : ntable_y = (ntable_y + TABLE_H - inc) % TABLE_H; break;
	case ALT_MASK | K_RIGHT : ntable_x = (ntable_x + inc) % TABLE_W; break;
	case ALT_MASK | K_DOWN  : ntable_y = (ntable_y + inc) % TABLE_H; break;

	//edit cursor
	case K_LEFT  : cursor_x = (cursor_x - inc) % BLOCK_SIZE; break;
	case K_UP    : cursor_y = (cursor_y - inc) % BLOCK_SIZE; break;
	case K_RIGHT : cursor_x = (cursor_x + inc) % BLOCK_SIZE; break;
	case K_DOWN  : cursor_y = (cursor_y + inc) % BLOCK_SIZE; break;


	//active color slot
	case K_CLOSE : active_slot = (active_slot + inc) % 4; break;
	case K_OPEN  : active_slot = (active_slot - inc) % 4; break;

	//active palette
	case SHIFT_MASK | K_CLOSE : active_pal = (active_pal + inc) % NB_PALS; break;
	case SHIFT_MASK | K_OPEN  : active_pal = (active_pal - inc) % NB_PALS; break;



	//draw
	case K_1 ... K_4 : set_pixel(EDIT_CHAR, cursor_x, cursor_y, key.keyCode - K_1); break;


	//change color
	case K_PLUS  : palette[active_pal][active_slot] = (palette[active_pal][active_slot] + inc) % PAL_SIZE; break;
	case K_MINUS : palette[active_pal][active_slot] = (palette[active_pal][active_slot] - inc) % PAL_SIZE; break;


	//cycle active bank
	case K_PGDN : active_bank = (active_bank + inc) % nb_banks;	break;
	case K_PGUP : active_bank = (active_bank - inc) % nb_banks;	break;


	//set tile
	case K_RETURN : nametable[ntable_x + ntable_y*TABLE_W] = select_x + select_y*MAP_SIZE; break;
	case K_DEL    : nametable[ntable_x + ntable_y*TABLE_W] = 0; break;


	//flip horizontally
	case K_F : break;

	//flip vertically
	case K_G : break;


	//point
	case K_P : {
		int i = nametable[ntable_x + ntable_y*TABLE_W];
		select_x = i % MAP_SIZE;
		select_y = i / MAP_SIZE;
	}
	break;



	//copy chr
	case K_C :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			clipboard[i] = chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]; 
		}
	break;


	//paste chr
	case K_V :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = clipboard[i]; 
		}
	break;



	//reload
	case SHIFT_MASK | K_Z :	reload_char(); break;
	case ALT_MASK   | K_Z : reload_nametable(); break;


	//save char map
	case SHIFT_MASK | K_S : {
		
		FILE* fp = fopen(chr_file_name, "w");

		if (fp != NULL) {
			printf("Saving char map %s\n", chr_file_name);
			fwrite(chr_data, 1, NB_TILES * nb_banks * BLOCK_SIZE * NB_PLANES, fp);
			fclose(fp);
		}
		else 
			WARNING("Couldn't write to file %s", chr_file_name);
	}
	break;



	//save char map
	case ALT_MASK | K_S : {

		
		FILE* fp = fopen(nt_file_name, "w");

		if (fp != NULL) {
			printf("Saving nametable %s\n", nt_file_name);
			fwrite(nametable, 1, TABLE_SIZE, fp);
			fclose(fp);
		}
		else 
			WARNING("Couldn't write nametable %s", nt_file_name);

	}
	break;



	//screenshot
	case K_TAB :
		printf("Saving screenshot.bmp\n");
		EZ_save_BMP(canvas, "./screenshot.bmp");
	break;


	//exit
	case K_ESCAPE : EZ_stop(); break; //exit


	default : 
		break;
	}


}
