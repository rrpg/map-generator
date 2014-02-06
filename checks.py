WATER = "water"
LAND = "land"
MOUNTAIN = "mountain"

GROUND_WATER   = 0x01
GROUND_LAND    = 0x04
GROUND_MOUNTAIN = 0x08

def isWater(groundtype):
	return groundtype == GROUND_WATER

def isLand(groundtype):
	return groundtype == GROUND_LAND

def isMountain(groundtype):
	return groundtype == GROUND_MOUNTAIN

def getGroundTypes():
	return {WATER: GROUND_WATER, LAND: GROUND_LAND, MOUNTAIN: GROUND_MOUNTAIN}
