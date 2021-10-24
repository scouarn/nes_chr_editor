#ifndef _COMMON_H_
#define _COMMON_H_

#include "ezGfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>



#define NB_TILES 256
#define NB_PLANES  2
#define NB_BANKS_DEFAULT 2 
#define BLOCK_SIZE 8
#define TABLE_SIZE 1024

#define EDIT_RES  32
#define MAP_SIZE  16
#define MAP_RES    2
#define PAL_SIZE  64
#define NB_PALS    4
#define PAL_RES   16
#define TABLE_W   32
#define TABLE_H   30
#define TABLE_RES  1


#define BORDER_COLOR EZ_WHITE
#define CURSOR_COLOR EZ_YELLOW

#define EDIT_CHAR (NB_TILES*active_bank + select_x + select_y*MAP_SIZE)

void reload_char();
void reload_nametable();
void set_pixel(unsigned int id, int x, int y, int col);



//globals
extern EZ_Image canvas;
extern EZ_Font  font;

extern char*  nt_file_name;
extern char* chr_file_name;
extern char* local;

extern uint cursor_x, cursor_y;
extern uint select_x, select_y;
extern uint ntable_x, ntable_y;

extern uint nb_banks;
extern uint active_bank;
extern uint active_slot;
extern uint active_pal;


extern EZ_Px NES_palette[];
extern uint8_t palette[NB_PALS][4];


extern uint8_t clipboard[];
extern uint8_t *chr_data;
extern uint8_t nametable[];



#endif //common_h