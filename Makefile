CC := gcc
CFLAGS := -Wall 

INCLUDE := ../ezGfx/include/
LIBS := -Lezgfx -lezgfx

TARGET := chredit
INSTALL := /usr/local/bin

SOURCES := $(wildcard source/*.c)
OBJECTS := $(patsubst source/%.c,bin/%.o,$(SOURCES))



all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(LIBS) -o $@ $^ 

bin/%.o : source/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ -c $<


install : $(TARGET)
	ln -sf `pwd`/chredit $(INSTALL)

clean :
	rm -f bin/*