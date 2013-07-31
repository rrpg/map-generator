#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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
unsigned int get_random_int(unsigned int max);

int main()
{
	time_t beginning;
	float map[HEIGHT][WIDTH];
	float min, max;

	beginning = time(NULL);
	min = max = 0.0;

	// set the random seed
	srand((unsigned)beginning);

	fillMap(map, &min, &max);
	printMap(map, min, max);

	printResult(beginning, time(NULL));

	return 1;
}

void fillMap(float map[HEIGHT][WIDTH], float *min, float *max)
{
	//set up some variables
	int octaves = 16;

	//these variables are used by the fBm part, but not extensively in the noise part
	float pixel_value,
		  amplitude, frequency,
		  gain = 0.65f, lacunarity = 2.0f;//physics terms. gain affects the amplitude each octave, lacunarity affects the frequency

	//these are all variables used only by the noise function, not the fBm part
	float disbx, disby,//distances to the three corners
		  dismx, dismy,//   b = bottom-left corner, m = middle corner, t = top-right corner
		  distx, disty,
		  noiseb,noisem,noiset,//noise contributions from the three corners
		  tempdis,x,y,
		  skew_value,unskew_value,
		  general_skew = (sqrt(3.0f)-1.0f)*0.5f, //these two are some complex math to convert between square and simplex space
		  general_unskew = (3.0f-sqrt(3.0f))/6.0f;

	int cornerbx, cornerby,
		cornermx, cornermy,
		cornertx, cornerty,
		gradb, gradm, gradt;//arrays should be used with all of these, but this is easier to read

	*min = 100000.0f;
	*max = -100000.0f;

	//set up the gradient table with 8 equally distributed angles around the unit circle
	float gradients[8][2];
	int i;
	for (i = 0; i < 8; ++i) {
		gradients[i][0] = cos(0.785398163f * (float)i);// 0.785398163 is PI/4.
		gradients[i][1] = sin(0.785398163f * (float)i);
	}

	int size = 256;
	int permutations[size];
	//set up the random numbers table
	// make it as long as the largest dimension
	for (i = 0; i < size; ++i) {
		// put each number in once
		permutations[i] = i;
	}

	//randomize the random numbers table
	int j, k;
	for (i = 0; i < size; ++i) {
		j = get_random_int(size);
		k = permutations[i];
		permutations[i] = permutations[j];
		permutations[j] = k;
	}

	//for each pixel...
	for (i = 0; i < HEIGHT; ++i) {
		for (j = 0; j < WIDTH; ++j) {
			//get the value for this pixel by adding successive layers
			amplitude = 1.0f;
			frequency = 1.0f / (float) WIDTH;
			pixel_value = 0.0f;

			// use threads here
			for (k = 0; k < octaves; ++k)
			{
				//get the x and y values. These are values from the grid in normal (simplex) space
				x = (float)j * frequency;
				y = (float)i * frequency;

				//get the bottom-left corner of the simplex in skewed space
				skew_value = (x + y) * general_skew;
				cornerbx = fasterFloor(x + skew_value);
				cornerby = fasterFloor(y + skew_value);

				//get the distance from the bottom corner in normal (simplex) space
				unskew_value = (float)(cornerbx + cornerby) * general_unskew;
				disbx = x - (float)cornerbx + unskew_value;
				disby = y - (float)cornerby + unskew_value;

				//get the middle corner in skewed space
				if (disbx > disby) {
					cornermx = 1 + cornerbx;// lower triangle
					cornermy = cornerby;
				}
				else {
					cornermx = cornerbx;//upper triangle
					cornermy = 1 + cornerby;
				}

				//get the top corner in skewed space
				cornertx = 1 + cornerbx;
				cornerty = 1 + cornerby;

				// get the distance from the other two corners
				dismx = disbx - (float)(cornermx - cornerbx) + general_unskew;
				dismy = disby - (float)(cornermy - cornerby) + general_unskew;

				distx = disbx - 1.0f + 2 * general_unskew;
				disty = disby - 1.0f + 2 * general_unskew;

				// get the gradients indices
				gradb = permutations[(cornerbx + permutations[cornerby & 255]) & 255] & 7;
				gradm = permutations[(cornermx + permutations[cornermy & 255]) & 255] & 7;
				gradt = permutations[(cornertx + permutations[cornerty & 255]) & 255] & 7;

				// get the noise from each corner using an attenuation function
				// first the bottom corner
				tempdis = 0.5f - disbx * disbx - disby * disby;
				if (tempdis < 0.0f) {
					noiseb = 0.0f;
				}
				else {
					noiseb = pow(tempdis, 4.0f) * dotproduct(gradients[gradb], disbx, disby);
				}

				// then the middle corner
				tempdis = 0.5f - dismx * dismx - dismy * dismy;
				if (tempdis < 0.0f) {
					noisem = 0.0f;
				}
				else {
					noisem = pow(tempdis, 4.0f) * dotproduct(gradients[gradm], dismx, dismy);
				}

				//  last the top corner
				tempdis = 0.5f - distx * distx - disty * disty;
				if (tempdis < 0.0f) {
					noiset = 0.0f;
				}
				else {
					noiset = pow(tempdis, 4.0f) * dotproduct(gradients[gradt], distx, disty);
				}

				// finally, add it in and adjust for the next layer
				// notice that no interpolation is needed, just straight summation
				pixel_value += (noiseb + noisem + noiset) * amplitude;

				amplitude *= gain;
				frequency *= lacunarity;
			}

			// put it in the map
			map[j][i] = pixel_value;

			// do some quick checks
			if (pixel_value < *min) {
				*min = pixel_value;
			}
			else if (pixel_value > *max) {
				*max = pixel_value;
			}
		}
	}
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

unsigned int get_random_int(unsigned int max)
{
	int r;
	float s;

	r = rand();
	s = (float)(r & 0x7fff)/(float)0x7fff;

	return (s * max);
}
