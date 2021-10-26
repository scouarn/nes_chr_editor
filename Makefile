CC := gcc
CFLAGS := -Wall 

INCLUDE := -Iezgfx/include/
LIBS := -Lezgfx -lezgfx

TARGET := chredit
INSTALL := /usr/local/bin

SOURCES := $(wildcard source/*.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))



all : $(TARGET)
$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^ 

%.o : %.c
	echo | gcc $(INCLUDE) -E -Wp,-v -
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<


.PHONY : install
install : $(TARGET)
	ln -sf `pwd`/chredit $(INSTALL)

clean :
	rm -f chredit source/*.o