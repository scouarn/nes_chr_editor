# NES char editor

Tile editor for NES character rom.


## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. If you want a symbolic link created in _/usr/local/bin_ run `sudo make install`. 


## Usage :

`chredit /path/to/file.chr`


## Controls :
| key 				| action 	  |
|-------------------|-------------|
| Arrow keys 		| move cursor |
| Numpad / enter 	| change tile |
| Page up / down 	| cycle active color |
| C / V 		 	| copy and paste |
| S 			 	| save |
| R 			 	| reload file |
| Escape 		 	| exit |
| Space 		 	| draw |
| X 			 	| erase |

-- Moving cursors can be done with the mouse. 