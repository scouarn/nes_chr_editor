# NES char editor

Tile editor for NES character rom.
Only works on linux (for now).

## TODO :
- Attribute table (flips, palette)
- Save nametable 
- Copy tile (+its attributes)
- 4 background palettes

- Support for char roms larger than 8k
- Controls config
- Portable local path/link solution -- config.h ?
- Implementation with a "mainstream"/"good" gfx lib
- Better editing tools (fill, swap colors, ...)


## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. Then if you want a symbolic link created in _/usr/local/bin_ run `sudo make install`. 


## Usage :

`chredit /path/to/file.chr`


## Controls :
| Key 				| Action 	  		  		|
|-------------------|---------------------------|
| Arrow keys 		| Move char edit cursor    	|
| Number keys 1-4 	| Draw 				  		|
| Shift+arrows	 	| Move char map cursor  	|
| C 			 	| Copy char 		  		|
| V 			 	| Paste char 		  		|
| Square brackets 	| Cycle active color  		|
| Plus / minus      | Change active color 		|
| Page up / down 	| Cycle chr bank      		|
| Alt+arrows		| Move nametable cursor 	|
| Return 			| Set tile to active char	|
| Delete 			| Set tile to zero			|
| F 				| Toggle tile hflip  		|
| G 				| Toggle tile vflip	  		|
| P 				| Point tile's char	id		|
| Ctrl 				| Increment of 4 	  		|
| Shift+S		 	| Save char map		  		|
| Shift+Z		 	| Reload char map	  		|
| Alt+S 			| Save nametable			|
| Alt+Z 			| Reload nametable 			|
| Tab 				| Screenshot (bmp)	  		|
| Escape 		 	| Exit 				  		|

-- Most actions can be done with the mouse and left click. 

