# NES char editor

Tile editor for NES character rom and nametable.
Only works on linux (for now).

## TODO :
- Find a better name
- Controls config
- Portable local path/link solution -- config.h ?
- Refactor
- Implementation with a "mainstream"/"good" gfx lib


## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. Then if you want a symbolic link created in _/usr/local/bin_ run `sudo make install`. 


## Usage :

`chredit -c charrom.chr  -n nametable.bin [-b nb_banks]`


## Controls :
| Key 				| Action 	  		  		|
|-------------------|---------------------------|
| Arrow keys 		| Move char edit cursor    	|
| Number keys 1-4 	| Draw 				  		|
| Ctrl+Number keys	| Fill with active color	|
| Shift+Number keys | Swap with active color	|
| D 				| Horizontal flip  			|
| F 				| Vertical flip	  			|
| Keypad 			| Shift by one pixel 	    |
| Plus / minus      | Change active color 		|
| Square brackets 	| Cycle active color  		|
| Shift+brackets 	| Cycle active palette		|
| Shift+arrows	 	| Move char map cursor  	|
| Page up / down	| Cycle char bank      		|
| Shift+C		 	| Copy char 		  		|
| Shift+V		 	| Paste char 		  		|
| Shift+S		 	| Save char map		  		|
| Shift+Z		 	| Reload char map	  		|
| Alt+arrows		| Move nametable cursor 	|
| Alt+Number keys   | Set meta tile palette 	|
| P 				| Point tile's char	id		|
| G 				| Toggle nametable grid		|
| Return 			| Set tile to active char	|
| Delete 			| Set tile to zero			|
| Alt+S 			| Save nametable			|
| Alt+Z 			| Reload nametable 			|
| Ctrl 				| Increment of 4 	  		|
| Tab 				| Screenshot (bmp)	  		|
| Escape 		 	| Exit 				  		|


-- Most actions can be done with the mouse and left click. 


## Palette configuration file :

The palette.conf file first defines in hex the 16 colors IDs used for the 4 default palette. It is followed by triplets of decimal integers defining the 64 RGB values for the colors available on the NES.  
