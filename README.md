# NES char editor

Tile editor for NES character rom.


## Dependencies :
- ezgfx https://github.com/scouarn/ezGfx
	(.so provided here, compiled under arch linux)


## Building / Installation :
Run `make clean all`. If you want a symbolic link created in /usr/local/bin run `sudo make install`. 


## Usage :

`chredit /path/to/file.chr`


## Controls :
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