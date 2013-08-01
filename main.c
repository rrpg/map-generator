#include <time.h>
#include <stdlib.h>
#include "map.h"

#define MAP_WIDTH 1000
#define MAP_HEIGHT 1000


int main()
{
	time_t beginning;
	float min, max;
	s_map map;
	int width, height;
	width = MAP_WIDTH;
	height = MAP_HEIGHT;

	beginning = time(NULL);
	min = max = 0.0;

	// set the random seed
	srand((unsigned)beginning);

	map = initMap(width, height);
	fillMap(&map, &min, &max);
	printMap(&map, min, max);

	printResult(beginning, time(NULL));
	free(map.grid);

	return 1;
}
