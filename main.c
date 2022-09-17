#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "map.h"

#define MAP_WIDTH 1000
#define MAP_HEIGHT 1000
#define DEFAULT_FILENAME "file"


int main(int argc, char* argv[])
{
	time_t seed;
	float min, max;
	s_map map;
	int c, pResult;
	char* file;
	short generateText;

	int width, height;
	width = MAP_WIDTH;
	height = MAP_HEIGHT;
	file = DEFAULT_FILENAME;
	generateText = 0;
	while ((c = getopt (argc, argv, "tw:h:f:")) != -1) {
		switch (c) {
			case 't':
				generateText = 1;
				break;
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
			case 'f':
				file = optarg;
				break;
			default:
				abort();
		}
	}

	seed = time(NULL);
	min = max = 0.0;

	// set the random seed
	srand((unsigned)seed);

	map = initMap(width, height);
	fillMap(&map, &min, &max);
	pResult = printMap(&map, min, max, file, strlen(file), generateText);

	free(map.grid);

	return pResult;
}
