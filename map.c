#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "color.h"
#include "map.h"


/*
 * Local functions
 */
int fasterFloor(float value);
float dotproduct(float grad[], float x, float y);
s_color lerp(s_color c1, s_color c2, float value);
unsigned int get_random_int(unsigned int max);

/**
 * Creates a s_map instance, set its dimensions and the global size of the grid
 */
s_map initMap(int width, int height)
{
	s_map map;
	map.width = width;
	map.height = height;

	map.grid = (s_cell*) malloc(height * width * sizeof(s_cell));

	return map;
}

/**
 * Frees the map's grid
 */
void clearMap(s_map* map)
{
	free((*map).grid);
}

/**
 * Generate the map using the simplex noise algorithm.
 */
void fillMap(s_map* map, float *min, float *max)
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
	for (j = 0; j < (*map).height; ++j) {
		for (i = 0; i < (*map).width; ++i) {
			//get the value for this pixel by adding successive layers
			amplitude = 1.0f;
			frequency = 1.0f / (float) (*map).width;
			pixel_value = 0.0f;

			// use threads here
			for (k = 0; k < octaves; ++k) {
				//get the x and y values. These are values from the grid in normal (simplex) space
				x = (float)i * frequency;
				y = (float)j * frequency;

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
			(*map).grid[i + j * (*map).width].x = i;
			(*map).grid[i + j * (*map).width].y = j;
			(*map).grid[i + j * (*map).width].altitude = pixel_value;

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

/**
 * Print the map in a BMP file
 */
int printMap(s_map* map, float min, float max, char* filename, int filename_len, short generateText)
{
	//set up some variables
	float diff = max - min,
		  flood = 0.5f,//flood level
		  mount = 0.85f;//mountain level

	flood *= diff;
	mount *= diff;

	int i,j,k;
	char bmpfile[filename_len + 4], txtfile[filename_len + 4];
	strcpy(bmpfile, filename);
	strcat(bmpfile, ".bmp");
	strcpy(txtfile, filename);
	strcat(txtfile, ".txt");

	//these can be changed for interesting results
	s_color waterlow, waterhigh, landlow, landhigh, mountlow, mounthigh;

	waterlow = color(0, 0, 55);
	waterhigh = color(0, 53, 106);
	landlow = color(0, 64, 0);
	landhigh = color(133, 182, 116);
	mountlow = color(167, 157, 147);
	mounthigh = color(216, 223, 226);

	//3.0 output to file
	//3.1 Begin the file
	//3.1.1 open output file
	FILE *bmp, *txt;
	bmp = fopen(bmpfile, "wb");

	if (generateText)
		txt = fopen(txtfile, "w");
	if (bmp == NULL || (generateText && txt == NULL)){
		printf("Target file opening error");
		return 1;
	}

	//3.1.2 copy the header
	//3.1.2.1 magic number
	fputc((char)66, bmp);
	fputc((char)77, bmp);

	//~//3.1.2.2 filsize/unused space
	for (i = 0; i < 8; i++) {
		fputc((char)0, bmp);
	}

	//~//3.1.2.3 data offset
	fputc((char)54, bmp);

	//~//3.1.2.4 unused space
	for (i = 0; i < 3; i++) {
		fputc((char)0, bmp);
	}

	//~//3.1.2.5 header size
	fputc((char)40, bmp);

	//~//3.1.2.6 unused space
	for (i = 0; i < 3; i++) {
		fputc((char)0, bmp);
	}

	//~//3.1.2.7 file width (trickier)
	fputc((char)((*map).width % 256), bmp);
	fputc((char)(((*map).width>>8)%256), bmp);
	fputc((char)(((*map).width>>16)%256), bmp);
	fputc((char)(((*map).width>>24)%256), bmp);

	//~//3.1.2.8 file height (trickier)
	fputc((char)((*map).height%256), bmp);
	fputc((char)(((*map).height>>8)%256), bmp);
	fputc((char)(((*map).height>>16)%256), bmp);
	fputc((char)(((*map).height>>24)%256), bmp);

	//~//3.1.2.9 color planes
	fputc((char)1, bmp);
	fputc((char)0, bmp);

	//~//3.1.2.10 bit depth
	fputc((char)24, bmp);

	//~//3.1.2.11 the rest
	for (i = 0; i < 25; i++) {
		fputc((char)0, bmp);
	}

	//3.2 put in the elements of the array
	s_color newcolor = color(0, 0, 0);
	s_cell* current;
	for (j = map->height - 1; j >= 0; j--) {//bitmaps start with the bottom row, and work their way up...
		for (i = 0; i < map->width; i++) {//...but still go left to right
			int directions, currentIndex;

			currentIndex = i + j * map->width;
			current = &(map->grid[currentIndex]);
			current->altitude -= min;
			//if this point is below the floodline...
			if (current->altitude < flood) {
				current->ground_type = GROUND_WATER;
				newcolor = lerp(waterlow, waterhigh, current->altitude / flood);
			}
			//if this is above the mountain line...
			else if (current->altitude > mount) {
				current->ground_type = GROUND_MOUNTAIN;
				newcolor = lerp(mountlow, mounthigh, (current->altitude - mount) / (diff - mount));
			}
			//if this is regular land
			else {
				current->ground_type = GROUND_LAND;
				newcolor = lerp(landlow, landhigh, (current->altitude - flood) / (mount - flood));
			}

			fputc((char)(newcolor.v[2]), bmp);//blue
			fputc((char)(newcolor.v[1]), bmp);//green
			fputc((char)(newcolor.v[0]), bmp);//red

			if (!generateText)
				continue;

			directions = 0;
			// WEST
			if (i > 0 && map->grid[currentIndex - 1].altitude >= flood) {
				directions |= BIT_CELL_WEST;
			}
			// EAST
			if (i < map->width - 1 && map->grid[currentIndex + 1].altitude - min >= flood) {
				directions |= BIT_CELL_EAST;
			}
			// NORTH
			if (j > 0 && map->grid[currentIndex - map->width].altitude - min >= flood) {
				directions |= BIT_CELL_NORTH;
			}
			// SOUTH
			if (j < map->height - 1 && map->grid[currentIndex + map->width].altitude >= flood) {
				directions |= BIT_CELL_SOUTH;
			}

			// NORTH WEST
			if (i > 0 && j > 0 && map->grid[currentIndex - 1 - map->width].altitude >= flood) {
				directions |= BIT_CELL_NORTH_WEST;
			}
			// NORTH EAST
			if (i < map->width - 1 && j > 0 && map->grid[currentIndex + 1 - map->width].altitude - min >= flood) {
				directions |= BIT_CELL_NORTH_EAST;
			}
			// SOUTH EAST
			if (i < map->width - 1 && j < map->height - 1 && map->grid[currentIndex + 1 + map->width].altitude - min >= flood) {
				directions |= BIT_CELL_SOUTH_EAST;
			}
			// SOUTH WEST
			if (i > 0 && j < map->height - 1 && map->grid[currentIndex -1 + map->width].altitude >= flood) {
				directions |= BIT_CELL_SOUTH_WEST;
			}
			fprintf(txt, "%d %d %d %d\n", (*current).ground_type, i, j, directions);
		}
		//round off the row
		for (k = 0; k < ((*map).width % 4); k++) {
			fputc((char)0, bmp);
		}
	}

	//3.3 end the file
	fclose(bmp);

	if (generateText)
		fclose(txt);

	return 0;
}

/**
 * Shows the time spent to generate the map, just for benchmarks, kindof dummy
 */
void printResult(time_t beginning, time_t end)
{
	printf("%d\n", (int) (end - beginning));
}

/**
 * Faster version of the floor function
 */
int fasterFloor(float value)
{
	return value >= 0 ? (int) value : (int) value - 1;
}

/**
 * Dotproduct between a gradient value and the current position
 */
float dotproduct(float grad[], float x, float y)
{
	return grad[0] * x + grad[1] * y;
}

/**
 * Calculate a color result of a linear interpolation from two colors and a
 * ratio
 */
s_color lerp(s_color c1, s_color c2, float value)
{
	s_color tcolor;
	tcolor.v[0] = tcolor.v[1] = tcolor.v[2] = 0;

	int g;
	for (g = 0; g < 3; g++) {
		tcolor.v[g] = MIN(c1.v[g], c2.v[g]) + (unsigned char) abs((float)(c2.v[g] - c1.v[g]) * value);
	}

	return tcolor;
}

/**
 * Generate pseudo random integer between 0 and max
 */
unsigned int get_random_int(unsigned int max)
{
	int r;
	float s;

	r = rand();
	s = (float)(r & 0x7fff)/(float)0x7fff;

	return (s * max);
}

