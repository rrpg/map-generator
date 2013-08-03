#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "map.h"

#define MAP_WIDTH 1000
#define MAP_HEIGHT 1000


int main(int argc, char* argv[])
{
	time_t beginning;
	float min, max;
	s_map map;
	int c, pResult;

	int width, height;
	width = MAP_WIDTH;
	height = MAP_HEIGHT;
	while ((c = getopt (argc, argv, "w:h:")) != -1) {
		switch (c) {
			case 'w':
				width = atoi(optarg);
				break;
			case 'h':
				height = atoi(optarg);
				break;
			default:
				abort();
		}
	}

	beginning = time(NULL);
	min = max = 0.0;

	// set the random seed
	srand((unsigned)beginning);

	map = initMap(width, height);
	fillMap(&map, &min, &max);
	pResult = printMap(&map, min, max);

	printResult(beginning, time(NULL));
	free(map.grid);

	return pResult;
}
