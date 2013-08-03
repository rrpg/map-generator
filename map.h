#define VISUAL_DISPLAY_MODE 0x01

typedef struct {
	float** grid;
	int width;
	int height;
} s_map;

s_map initMap(int width, int height);
void clearMap(s_map* map);
void fillMap(s_map* map, float *min, float *max);
int printMap(s_map* map, float min, float max);
void printResult(time_t beginning, time_t end);

#define MIN(a, b) (a < b ? a : b)
