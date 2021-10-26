#include "common.h"


void draw_block(int id, int x0, int y0, int res, int pal) {


	unsigned char* plane1 = chr_data + (id * BLOCK_SIZE * NB_PLANES);
	unsigned char* plane2 = plane1 + BLOCK_SIZE;

	for (int y = 0; y < BLOCK_SIZE; y++)
	for (int x = 0; x < BLOCK_SIZE; x++) {

		//take to two bits corresponding to the color information
		unsigned char col_id = 0;

		col_id |= (  plane1[y] >> x) & 0x1;
		col_id |= (( plane2[y] >> x) & 0x1) << 1;

		EZ_Px col = NES_palette[palette[pal][col_id]];

		EZ_draw2D_fillRect(canvas, col, x0 + (BLOCK_SIZE - x - 1)*res, y0 + y*res, res, res);

	}

}

void EZ_callback_draw(double dt) {

	//clear
	EZ_draw2D_clear(canvas, EZ_BLUE);
	EZ_draw2D_setTranslate(0, 0);




	//edit char
	draw_block(EDIT_CHAR, 0, 0, EDIT_RES, active_pal);


	//char map
	for (int x = 0; x < MAP_SIZE; x++)
	for (int y = 0; y < MAP_SIZE; y++) {

		draw_block(active_bank*NB_TILES + x + y*MAP_SIZE, 
			x*BLOCK_SIZE*MAP_RES, 
			canvas.h/2 + y*BLOCK_SIZE*MAP_RES, 
			MAP_RES, 
			active_pal
		);

	}

	//nametable
	for (int x = 0; x < TABLE_W; x++)
	for (int y = 0; y < TABLE_H; y++) {

		uint tile = 960 + x/4 + (y / 4)*8;
		uint offset = x%4/2 + (y%4/2)*2;
		uint col = (nametable[tile] >> offset*2) & 0b11;

		draw_block(NB_TILES*active_bank + nametable[x + y*TABLE_W], 
			canvas.w/2 + x*BLOCK_SIZE*TABLE_RES, 
			canvas.h/2 + y*BLOCK_SIZE*TABLE_RES, 
			1,
			col
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
	int col = palette[active_pal][active_slot];

	int x = col % 16;
	int y = col / 16;


	//current 4 color palette (with IDs)
	for (int i = 0; i < NB_PALS; i++)
	for (int j = 0; j < 4;       j++) {

		//color square
		EZ_draw2D_fillRect(canvas, NES_palette[palette[i][j]],
			canvas.w/4*3 + j*PAL_RES*2, 
			PAL_RES*4    + i*PAL_RES*2,
			PAL_RES*2,
			PAL_RES*2 

		);

		//color id
		char buffer[4];
		sprintf(buffer, "%02X", palette[i][j]);
		EZ_draw2D_printStr(
			canvas, buffer, font, EZ_WHITE, EZ_BLUE, 
			canvas.w/4*3 + j*PAL_RES*2, 
			PAL_RES*4    + i*PAL_RES*2,
			2, 1
		);

	}

	//palette ids 
	for (int i = 0; i < NB_PALS; i++) {

		char buffer[5];
		sprintf(buffer, "PAL%d", i);
		EZ_draw2D_printStr(
			canvas, buffer, font, EZ_WHITE, EZ_BLUE, 
			canvas.w/4*3 - 5*font.w_px, 
			PAL_RES*4 + i*PAL_RES*2,
			4, 1
		);

	}

	//meta grid

	if (show_meta_grid)
	for (int i = 0; i < TABLE_W; i += 4) {
		EZ_draw2D_line(canvas, BORDER_COLOR, 
			canvas.w/2 + i*BLOCK_SIZE, 
			canvas.h/2, 
			canvas.w/2 + i*BLOCK_SIZE,
			canvas.h
		);

		EZ_draw2D_line(canvas, BORDER_COLOR, 
			canvas.w/2, 
			canvas.h/2 + i*BLOCK_SIZE, 
			canvas.w,
			canvas.h/2 + i*BLOCK_SIZE
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
		canvas.w/4*3 + active_slot*PAL_RES*2, 
		PAL_RES*4  + PAL_RES*2*active_pal,
		PAL_RES*2, PAL_RES*2
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

	sprintf(buffer, "Editing $%02X Bank %d\n"
					"Tile %02d,%02d : $%02X\n"
					"Meta %d,%d : PAL%d",

		EDIT_CHAR & 0xff, 
		active_bank,

		ntable_x,
		ntable_y,
		nametable[ntable_x + ntable_y*TABLE_W],

		ntable_x/2,
		ntable_y/2,
		(nametable[META_ID] >> META_OFFSET*2) & 0b11
	);

	EZ_draw2D_printStr(canvas, buffer, font, 
			BORDER_COLOR, EZ_BLUE, 
			canvas.w/2 + font.w_px, 
			canvas.h/2 - font.h_px*4,
			31, 4
	);


}
