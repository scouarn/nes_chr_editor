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

int get_pixel(unsigned int id, int x, int y) {
	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	int offset = BLOCK_SIZE - x - 1;

	int col = ((plane1[y] >> offset) &  1)
			| ((plane2[y] >> offset) << 1);

	return col & 0b11;

}



void swap(int col1, int col2) {


	for (int i = 0; i < BLOCK_SIZE; i++)
	for (int j = 0; j < BLOCK_SIZE; j++) {
		if (col1 == get_pixel(EDIT_CHAR, i, j))
			set_pixel(EDIT_CHAR, i, j, col2);

		else if (col2 == get_pixel(EDIT_CHAR, i, j))
			set_pixel(EDIT_CHAR, i, j, col1);

	}

}

void fill(int x, int y, int rep, int ori) {

	if (x < 0 || x >= BLOCK_SIZE || y < 0 || y >= BLOCK_SIZE)
		return;
	
	
	int col = get_pixel(EDIT_CHAR, x, y);
	if (col != ori || col == rep)
		return;

	set_pixel(EDIT_CHAR, x, y, rep); 

	fill(x+1, y, rep, ori);
	fill(x-1, y, rep, ori);
	fill(x, y+1, rep, ori);
	fill(x, y-1, rep, ori);

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



	//draw/fill
	case K_1 ... K_4 :

		if (ctrl) //harder than using CTRL_MASK 
			fill(cursor_x, cursor_y, key.keyCode - K_1, get_pixel(EDIT_CHAR, cursor_x, cursor_y)); 
		else 
			set_pixel(EDIT_CHAR, cursor_x, cursor_y, key.keyCode - K_1); 
		
	break;

	//swap palette
	case SHIFT_MASK | K_1 ... SHIFT_MASK | K_4 :
		swap(key.keyCode - K_1, active_slot); 
	break;

	//nametable set palette
	case ALT_MASK | K_1 ... ALT_MASK | K_4 :
		nametable[META_ID] &= ~(0b11 << META_OFFSET*2);
		nametable[META_ID] |= ((key.keyCode - K_1) & 0b11) << META_OFFSET*2;
	break;


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
	case K_D : 
		for (int i = 0; i < BLOCK_SIZE*NB_PLANES; i++) {

			uint8_t tmp = chr_data[i + EDIT_CHAR*BLOCK_SIZE*NB_PLANES];
			chr_data[i + EDIT_CHAR*BLOCK_SIZE*NB_PLANES] = 0;

			//bit back the bits backward
			for (int j = 0; j < BLOCK_SIZE; j++)
				chr_data[i + EDIT_CHAR*BLOCK_SIZE*NB_PLANES] |= ((tmp >> j) & 1) << (BLOCK_SIZE-j-1);
		}
	break;

	//flip vertically
	case K_F : 
		for (int i = 0; i < BLOCK_SIZE/2; i++) {

			//plane 1
			SWAP(chr_data[i + EDIT_CHAR*BLOCK_SIZE*NB_PLANES],
				 chr_data[BLOCK_SIZE-i-1 + EDIT_CHAR*BLOCK_SIZE*NB_PLANES]
				);
				
			//plane 2
			SWAP(chr_data[BLOCK_SIZE+i + EDIT_CHAR*BLOCK_SIZE*NB_PLANES],
				 chr_data[BLOCK_SIZE*2-i-1 + EDIT_CHAR*BLOCK_SIZE*NB_PLANES]
				);
		}
	break;

	//shift down
	case KP_2 :
		for (int i = BLOCK_SIZE-1; i >= 1; i--) {
			chr_data[i   + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]
		  = chr_data[i-1 + EDIT_CHAR * BLOCK_SIZE * NB_PLANES];

			chr_data[i   + BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]
		  = chr_data[i-1 + BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES];
		}
		chr_data[EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = 0;
		chr_data[BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = 0;
	break;

	//shift up
	case KP_8 :
		for (int i = 0; i < BLOCK_SIZE-1; i++) {
			chr_data[i   + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]
		  = chr_data[i+1 + EDIT_CHAR * BLOCK_SIZE * NB_PLANES];

			chr_data[i   + BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]
		  = chr_data[i+1 + BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES];
		}
		chr_data[BLOCK_SIZE-1 + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = 0;
		chr_data[BLOCK_SIZE-1 + BLOCK_SIZE + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] = 0;
	break;

	//shift left
	case KP_4 :
		for (int i = 0; i < BLOCK_SIZE*NB_PLANES; i++) {
			chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] <<= 1;
		}
	break;

	//shift right
	case KP_6 :
		for (int i = 0; i < BLOCK_SIZE*NB_PLANES; i++) {
			chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES] >>= 1;
		}
	break;

	//toggle grid
	case K_G : show_meta_grid ^= true; break;


	//point
	case K_P : {
		int i = nametable[ntable_x + ntable_y*TABLE_W];
		select_x = i % MAP_SIZE;
		select_y = i / MAP_SIZE;
	}
	break;



	//copy chr
	case SHIFT_MASK | K_C :
		for (int i = 0; i < BLOCK_SIZE * NB_PLANES; i++) {
			clipboard[i] = chr_data[i + EDIT_CHAR * BLOCK_SIZE * NB_PLANES]; 
		}
	break;


	//paste chr
	case SHIFT_MASK | K_V :
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
