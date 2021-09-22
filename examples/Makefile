CC := tcc
W := -Wall -Werror
L := -lm -lSDL2 -lSDL2_ttf -lSDL2_image

all: main

main: main.c
	$(CC) -o main main.c -lPAT $(L) $(W)

.PHONY: clean
clean:
	rm main
