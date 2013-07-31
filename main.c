#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000

typedef struct {
	// v[0]=red, v[1]=green, v[2]=blue
	// Array used to be able to loop on it
	unsigned char v[3];
} s_color;

void fillMap(float map[HEIGHT][WIDTH], float min, float max);
void printMap(float map[HEIGHT][WIDTH], float min, float max);
void printResult(time_t beginning, time_t end);

int main()
{
	time_t beginning;
	float map[HEIGHT][WIDTH];
	float min, max;

	beginning = time(NULL);
	min = max = 0.0;

	fillMap(map, min, max);
	printMap(map, min, max);

	printResult(beginning, time(NULL));

	return 1;
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


