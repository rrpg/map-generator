# Map generator - Simplex noise

C Implementation of the Simplex algorithm to randomly generate maps.
This is an adaptation of this c++ version: https://code.google.com/p/fractalterraingeneration/downloads/detail?name=simplexnoise.1.0.cpp&can=2&q=

## Usage

Compile it:
```
make
```

And run it:
```
./map
```

You can use the following arguments:
```
# Set the map width (in pixels), default value is 1000
./map -w %d

# Set the map height (in pixels), default value is 1000
./map -h %d

# Set the filename where the map will be drawn (BMP file), default value is file.bmp
./map -f %s
```

## Result

You can find some generation examples in the examples folder.

## What's next?

- I'll add another option to return the map details to be usable by another
program for example.
- I'd like to add beaches in the map, between the water and the land.
- I'd like to add deserts, depending on the Y value of the pixels in the map (
deserts will be in the vertical center of the map)
