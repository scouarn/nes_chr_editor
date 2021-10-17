CC = gcc
CFLAGS = -Wall 

INCLUDE = ../ezGfx/include/
LIBS = -lm -lpthread -lasound -lX11


TARGET = bin/main


SOURCES = $(wildcard source/*.c)
OBJECTS = $(patsubst source/%.c,bin/%.o,$(SOURCES))

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $^ $(LIBS)  -Lbin -lezgfx



bin/%.o : source/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $<

clean :
	rm -f bin/*