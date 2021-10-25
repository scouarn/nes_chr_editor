# NES char editor

Tile editor for NES character rom and nametable.
Only works on linux (for now).

## TODO :
- Attribute table (flips, palette,...)
- Copy tile (+its attributes)

- Find a better name

- Controls config
- Portable local path/link solution -- config.h ?
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
| Alt+Number keys   | Fill 						|
| Shift+Number keys | Swap with active color	|
| Plus / minus      | Change active color 		|
| Square brackets 	| Cycle active color  		|
| Shift+brackets 	| Cycle active palette		|
| Shift+arrows	 	| Move char map cursor  	|
| Shift+C		 	| Copy char 		  		|
| Shift+V		 	| Paste char 		  		|
| Page up / down	| Cycle char bank      		|
| Shift+S		 	| Save char map		  		|
| Shift+Z		 	| Reload char map	  		|
| Alt+arrows		| Move nametable cursor 	|
| Alt+S 			| Save nametable			|
| Alt+Z 			| Reload nametable 			|
| Return 			| Set tile to active char	|
| Delete 			| Set tile to zero			|
| F 				| Toggle tile hflip  		|
| G 				| Toggle tile vflip	  		|
| P 				| Point tile's char	id		|
| Ctrl 				| Increment of 4 	  		|
| Tab 				| Screenshot (bmp)	  		|
| Escape 		 	| Exit 				  		|


-- Most actions can be done with the mouse and left click. 

