CC := gcc
INSTALL_LIB_DIR := /usr/lib
INSTALL_INCLUDE_DIR := /usr/include
W := -Wall -Werror
L := -lm -lSDL2 -lSDL2_ttf -lSDL2_image

all: PAT

PAT: PAT.h PAT.c
	$(CC) -c -fPIC PAT.c $(W)
	$(CC) -shared -o libPAT.so PAT.o $(L)
	rm PAT.o

install:
	cp libPAT.so $(INSTALL_LIB_DIR)
	cp PAT.h $(INSTALL_INCLUDE_DIR)

uninstall:
	rm $(INSTALL_LIB_DIR)/libPAT.so
	rm $(INSTALL_INCLUDE_DIR)/PAT.h

.PHONY: examples
examples:
	cd examples && make && cd ..

.PHONY: clean
clean:
	rm *.so

.PHONY: clean_all
clean_all:
	make clean
	cd examples && make clean && cd ..
