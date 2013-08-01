all: main.c map.c color.c
	gcc -lm -Wall -o "map" main.c map.c color.c
