typedef struct {
	// v[0]=red, v[1]=green, v[2]=blue
	// Array used to be able to loop on it
	unsigned char v[3];
} s_color;

s_color color(int r, int g, int b);
