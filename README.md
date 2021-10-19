# NES char editor

Tile editor for NES character rom.


## TODO :
- Better editing tools (fill, swap colors)
- Multiple saved palettes
- Support for char roms larger than 8k / with variable size
- Meta sprite view
- Controls config ==>> numpad not portable
- Implementation with a "mainstream"/"good" gfx lib

## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. Then if you want a symbolic link created in _/usr/local/bin_ run `sudo make install`. 


## Usage :

`chredit /path/to/file.chr`


## Controls :
| Key 				| Action 	  		  |
|-------------------|---------------------|
| Numpad		 	| Change tile  		  |
| Arrow keys 		| Move cursor 		  |
| Number keys 1-4 	| Draw 				  |
| F 				| Toggle hflip		  |
| G 				| Toggle vflip	 	  |
| Page up / down 	| Cycle active color  |
| Plus / minus      | Change active color |
| Ctrl 				| Increment of 4 	  |
| C 			 	| Copy tile 		  |
| V 			 	| Paste tile 		  |
| S 			 	| Save file			  |
| R 			 	| Reload file 		  |
| Tab 				| Screenshot (bmp)	  |
| Escape 		 	| Exit 				  |

-- Most actions can be done with the mouse and left click. 

