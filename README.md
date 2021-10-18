# NES char editor

Tile editor for NES character rom.


## TODO :
- Better editing tools (fill, paint with number keys...)
- Change palette with keyboard
- Multi page support (for char roms larger than 4k)


## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. If you want a symbolic link created in _/usr/local/bin_ run `sudo make install`. 


## Usage :

`chredit /path/to/file.chr`


## Controls :
| key 				| action 	  		  |
|-------------------|---------------------|
| Arrow keys 		| move cursor 		  |
| Number keys 1-4 	| draw 				  |
| Numpad		 	| change tile  		  |
| Page up / down 	| cycle active color  |
| Plus / minus      | change active color |
| C / V 		 	| copy and paste tile |
| S 			 	| save file			  |
| R 			 	| reload file 		  |
| Escape 		 	| exit 				  |

-- Most actions can be done with the mouse and left clic. 

