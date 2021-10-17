
/*
/
/	EZGFX : MY IDEAL C CROSSPLATFORM GRAPHICS ENGINE HEADER
/ Abstracts platforms and graphics APIs.
/
/ SCOUARN, 2021
/
/	Creates a single window with specified size and attached writable image to draw.
/ Handles keyboard and mouse inputs.
/
/
*/


#ifndef EZGFX_CORE
#define EZGFX_CORE


#include "ezGfx_utils.h"


/* MAIN FUNCTIONS */
void EZ_start();	//start the app thread
void EZ_stop();		//force it to stop
void EZ_join(); 	//wait for it to stop


/* INPUT FUNCTIONS */



enum EZ_KeyCode {
	K_ERROR,
	K_LMB, K_RMB, K_MMB,
	K_A, K_B, K_C, K_D, K_E, K_F, K_G, K_H, K_I, K_J, K_K, K_L, K_M, K_N, K_O, K_P, K_Q, K_R, K_S, K_T, K_U, K_V, K_W, K_X, K_Y, K_Z,
	K_0, K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, K_9,
	K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_F7, K_F8, K_F9, K_F10, K_F11, K_F12,
	K_SPACE, K_TAB, K_BACKSPACE, K_RETURN, K_ESCAPE,
	K_CAPS, K_LSHIFT, K_RSHIFT, K_LCTRL, K_RCTRL, K_LALT, K_RALT,
	K_INS, K_DEL, K_END, K_HOME, K_PGUP, K_PGDN, K_SCROLL, K_PAUSE, K_NUMLOCK,
	K_LEFT, K_UP, K_RIGHT, K_DOWN,
	K_COLON, K_COMMA, K_PERIOD, K_QUOTE, K_SLASH, K_BACKSLASH, K_TILDE, K_OPEN, K_CLOSE, K_PLUS, K_MINUS,
	KP_0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9, KP_DIV, KP_MUL, KP_PLUS, KP_MINUS, KP_ENTER, KP_DEC,
	_numberOfKeys
};

typedef struct {
	bool pressed;	//has it been pressed on this frame ?
	bool released;	//has it been released on this frame ?
	bool held;		//is it still held ?
	char typed;		//the actual character typed ('\0' if not ascii)
	enum EZ_KeyCode  keyCode;
} EZ_Key;

typedef struct {
	int x;		//position in canvas space
	int y;
	int dx;		//change in position since last frame 
	int dy;
	int wheel;	// +1 or -1
} EZ_Mouse;

/* The implementation keeps track of keyboard and mouse state	*/
EZ_Key   EZ_getKey(enum EZ_KeyCode code);
EZ_Mouse EZ_getMouse();


/* Callback functions left to the client to define */
extern void EZ_callback_init();						//at the start of the thread, before first frame
extern void EZ_callback_draw(double dt);			//called at each frame to update the canvas
extern void EZ_callback_kill();						//after the last frame (free memory...)
extern void EZ_callback_keyPressed(EZ_Key key);		//when a key is pressed
extern void EZ_callback_keyReleased(EZ_Key key);	//when a key is released
extern void EZ_callback_mouseMoved(EZ_Mouse mouse); //when the mouse is moved


/* COLOR FUNCITONS */
typedef union {
	uint32_t ref;  //the 32bits "hex" value
	struct {uint8_t a, b, g, r;};
} EZ_Px;

typedef struct {
	uint32_t w, h;	//size in pixels
	EZ_Px* px;	//pixel array
	
	void* reserved; //left to implementation

} EZ_Image;


EZ_Image EZ_createImage(int w, int h); //allocates memory for an image of given size
EZ_Image EZ_copyImage(EZ_Image);	   //allocates memory and copies data to new image
void  	 EZ_freeImage(EZ_Image);	   //free allocated memory



typedef struct {
	int h_px; //size in pixels of a character
	int w_px;
	int w_bytes; //number of bytes in each row
	uint8_t** data;   //array of pointers : one per char pointing to corresponding pixel information

} EZ_Font;

EZ_Font EZ_createFont(int w, int h); //allocates memory for 256 chars with given size in pixels
void    EZ_freeFont(EZ_Font font);



enum EZ_BlendMode {ALPHA_BLEND,  //standart alpha blending
				   ALPHA_IGNORE, //no blending at all
				   ALPHA_FAST	 //ignore alpha if bigger than 0
				};

EZ_Px EZ_blend(EZ_Px fg, EZ_Px bg, enum EZ_BlendMode mode);

EZ_Px EZ_randCol();	//random white noise RGBA color


#define EZ_BLACK   ((EZ_Px)0x000000FFU) //predefined colors
#define EZ_WHITE   ((EZ_Px)0xFFFFFFFFU)
#define EZ_BLUE    ((EZ_Px)0x0000FFFFU)
#define EZ_GREEN   ((EZ_Px)0x00FF00FFU)
#define EZ_CYAN    ((EZ_Px)0x00FFFFFFU)
#define EZ_RED     ((EZ_Px)0xFF0000FFU)
#define EZ_MAGENTA ((EZ_Px)0xFF00FFFFU)
#define EZ_YELLOW  ((EZ_Px)0xFFFF00FFU)

#define EZ_ORANGE  	((EZ_Px)0xFF7F00FFU)
#define EZ_LIME  	((EZ_Px)0x7FFF00FFU)
#define EZ_PURPLE  	((EZ_Px)0x7F00FFFFU)
#define EZ_GRAY		((EZ_Px)0x7F7F7FFFU)


#define EZ_RGB(r,g,b) 	 ((EZ_Px){255, b, g, r})
#define EZ_RGBA(r,g,b,a) ((EZ_Px){  a, b, g, r})
#define EZ_BW(c) 		 ((EZ_Px){255, c, c, c}) //grayscale color



/* UTILITY */
void EZ_bind(EZ_Image canvas); 		//set what should be drawn
void EZ_rename(const char*);  		//change the name of the window
void EZ_resize(int winW, int winH);	//change the size of the window

void EZ_redraw(); //force screen redraw (will call the draw callback)

void EZ_setMaximized(bool);  //maximize window
void EZ_setFullscreen(bool); //window fullscreen

double EZ_getTime(); //returns time in seconds since start of the thread






#endif
