#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "map.h"

#define MAP_WIDTH 1000
#define MAP_HEIGHT 1000
#define DEFAULT_FILENAME "file"


int main(int argc, char* argv[])
{
	time_t seed;
	s_map map;
	int c, pResult;
	char* file;

	int width, height, x, y;
	width = MAP_WIDTH;
	height = MAP_HEIGHT;
	x = 0;
	y = 0;
	seed = time(NULL);
	file = DEFAULT_FILENAME;
	while ((c = getopt (argc, argv, "w:x:y:s:h:f:")) != -1) {
		switch (c) {
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
			case 'x':
				x = atoi(optarg);
				break;
			case 'y':
				y = atol(optarg);
				break;
			case 's':
				seed = atoi(optarg);
				break;
			case 'f':
				file = optarg;
				break;
			default:
				abort();
		}
	}

	printf("Seed: %ld\n", seed);

	// set the random seed
	srand((unsigned)seed);

	map = initMap(width, height, x, y);
	fillMap(&map);
	pResult = printMap(&map, file, strlen(file));
	if (pResult == 0) {
		pResult = exportMapToTiled(&map, file, strlen(file));
	}

	free(map.grid);

	return pResult;
}
