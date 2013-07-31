#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000

typedef struct {
	// v[0]=red, v[1]=green, v[2]=blue
	// Array used to be able to loop on it
	unsigned char v[3];
} s_color;

void fillMap(float map[HEIGHT][WIDTH], float *min, float *max);
void printMap(float map[HEIGHT][WIDTH], float min, float max);
void printResult(time_t beginning, time_t end);
int fasterFloor(float value);
float dotproduct(float grad[], float x, float y);
s_color lerp(s_color c1, s_color c2, float value);

int main()
{
	time_t beginning;
	float map[HEIGHT][WIDTH];
	float min, max;

	beginning = time(NULL);
	min = max = 0.0;

	fillMap(map, &min, &max);
	printMap(map, min, max);

	printResult(beginning, time(NULL));

	return 1;
}

void fillMap(float map[HEIGHT][WIDTH], float *min, float *max)
{

}

void printMap(float map[HEIGHT][WIDTH], float min, float max)
{

}

void printResult(time_t beginning, time_t end)
{

}


int fasterFloor(float value)
{
	return value >= 0 ? (int) value : (int) value - 1;
}

float dotproduct(float grad[], float x, float y)
{
	return grad[0] * x + grad[1] * y;
}

s_color lerp(s_color c1, s_color c2, float value)
{
	s_color tcolor;
	tcolor.v[0] = tcolor.v[1] = tcolor.v[2] = 0;

	int g;
	for (g = 0; g < 3; g++) {
		if (c1.v[g] > c2.v[g]) {
			tcolor.v[g] = c2.v[g] + (unsigned char) ((float)(c1.v[g] - c2.v[g]) * value);
		}
		else {
			tcolor.v[g] = c1.v[g] + (unsigned char) ((float)(c2.v[g] - c1.v[g]) * value);
		}
	}

	return tcolor;
}

