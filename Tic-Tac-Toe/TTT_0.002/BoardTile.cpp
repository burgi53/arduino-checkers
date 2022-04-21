
#include "BoardTile.h"

//Creating a class constructor
BoardTile::BoardTile() {
	TileSizeX = 0;
	TileSizeY = 0;
	TileColor = 0;
	TileLocationX = 0;
	TileLocationY = 0;
	PieceValue = 0;
}

BoardTile::BoardTile(int tsx, int tsy, int tc, int tlx, int tly, int pv) {
	TileSizeX = tsx;
	TileSizeY = tsy;
	TileColor = tc;
	TileLocationX = tlx;
	TileLocationY = tly;
	PieceValue = pv;
}

BoardTile::~BoardTile() {

}

void BoardTile::setTileSizeXY(int X, int Y) {
	if (X >= 0 && Y >= 0) {
		TileSizeX = X;
		TileSizeY = Y;
	}
}
//setTileSize refers to pixel count in tile size
void BoardTile::setTileSizeX(int Tile_X) {
	if (Tile_X >= 0) {
		TileSizeX = Tile_X;
	}
	else {
		TileSizeX = 0;
	}
 }

void BoardTile::setTileSizeY(int Tile_Y) {
	if (Tile_Y >= 0) {
		TileSizeX = Tile_Y;
	}
	else {
		TileSizeY = 0;
	}
}

void BoardTile::setTileColor(int t_color) {
	if (TileColor >= 0) {
		TileColor = t_color;
	}
	else {
		TileColor = 0;
	}
}

//TileLocation refers to position of tile in the board array
void BoardTile::setTileLocation(int x, int y) {
	if (x >= 0 && y >= 0) {
		TileLocationX = x;
		TileLocationY = y;
	}
	else {
		TileLocationX = 0;
		TileLocationY = 0;
	}
}

void BoardTile::setTileLocationX(int x) {
	if (x >= 0) {
		TileLocationX = x;
	}
	else {
		TileLocationX = 0;
	}
}

void BoardTile::setTileLocationY(int y) {
	if (y >= 0) {
		TileLocationY = y;
	}
	else {
		TileLocationY = 0;
	}
}

//setPiecevalue assigns the type of piece thats associated with the tile
//i.e. for tictactoe a 1 is an x and an o is (-1)
void BoardTile::setPieceValue(int pVal) {
	if (pVal >= 0) {
		PieceValue = pVal;
	}
	else {
		PieceValue = pVal;
	}
}

int BoardTile::getTileSizeXY() {
	return TileSizeX, TileSizeY;
}

int BoardTile::getTileSizeX() {
	return TileSizeX;
}

int BoardTile::getTileSizeY() {
	return TileSizeY;
}

int BoardTile::getTileLocationXY() {
	return TileLocationX, TileLocationY;
}

int BoardTile::getTileLocationX() {
	return TileLocationX;
}

int BoardTile::getTileLocationY() {
	return TileLocationY;
}

int BoardTile::getTileColor() {
	return TileColor;
}

int BoardTile::getPieceValue() {
	return PieceValue;
}
