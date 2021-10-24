#include "common.h"


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

	sprintf(buffer, "Editing $%02X\nTile at %d, %d : $%02X\n",
		EDIT_CHAR, 
		ntable_x, 
		ntable_y, 
		nametable[ntable_x + ntable_y*TABLE_W]
	);

	EZ_draw2D_printStr(canvas, buffer, font, 
			BORDER_COLOR, EZ_BLUE, 
			canvas.w/2 + font.w_px, 
			canvas.h/4,
			24, 16
	);


}
