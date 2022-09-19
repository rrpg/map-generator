#define VISUAL_DISPLAY_MODE 0x01
#define GROUND_WATER 0x01
#define GROUND_LAND 0x04
#define GROUND_MOUNTAIN 0x08
#define GROUND_SNOW 0x10

#define BIT_CELL_NORTH  0x01
#define BIT_CELL_EAST   0x02
#define BIT_CELL_SOUTH  0x04
#define BIT_CELL_WEST   0x08
#define BIT_CELL_NORTH_WEST 0x10
#define BIT_CELL_NORTH_EAST 0x20
#define BIT_CELL_SOUTH_EAST 0x40
#define BIT_CELL_SOUTH_WEST 0x80

#define MAX_ALTITUDE 5000.0

typedef struct {
	int x;
	int y;
	float altitude;
	int ground_type;
} s_cell;

typedef struct {
	s_cell* grid;
	int width;
	int height;
	int x;
	int y;
	float minAltitude;
	float maxAltitude;
	float floodAltitude;
	float mountAltitude;
	float snowAltitude;
} s_map;

s_map initMap(int width, int height, int x, int y);
void clearMap(s_map* map);
void fillMap(s_map* map);
int printMap(s_map* map, char* filename, int filename_len, short generateText);
void printResult(time_t beginning, time_t end);

#define MIN(a, b) (a < b ? a : b)
