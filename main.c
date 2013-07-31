#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000

void fillMap(float map[HEIGHT][WIDTH], float min, float max);
void printMap(float map[HEIGHT][WIDTH], float min, float max);
void printResult(time_t beginning, time_t end);

int main()
{
	time_t beginning;
	float map[HEIGHT][WIDTH];
	float min, max;

	beginning = time(NULL);

	fillMap(map, min, max);
	printMap(map, min, max);

	printResult(beginning, time(NULL));
}

void fillMap(float map[HEIGHT][WIDTH], float min, float max)
{

}

void printMap(float map[HEIGHT][WIDTH], float min, float max)
{

}

void printResult(time_t beginning, time_t end)
{

}


