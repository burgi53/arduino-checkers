/*
* File: BoardTile.h
* Author: aBurgess
* 
* Created: February 2nd, 2022
*/

#ifndef BOARDTILE_H
#define BOARDTILE
#endif // !BOARDTILE_H

class BoardTile
{
private:
	int TileSizeX;
	int TileSizeY;
	int TileColor;
	int TileLocationX;
	int TileLocationY;
	int PieceValue;

public:

	BoardTile();

	BoardTile(int, int, int, int, int, int);

	~BoardTile(); 

	//setters-----
	void setTileSizeXY(int, int);
	void setTileSizeY(int);
	void setTileSizeX(int);

	void setTileColor(int);

	void setTileLocation(int, int);
	void setTileLocationY(int);
	void setTileLocationX(int);

	void setPieceValue(int);

	//getters--
	int getTileSizeXY();
	int getTileSizeX();
	int getTileSizeY();

	int getTileLocationXY();
	int getTileLocationX();
	int getTileLocationY();

	int getTileColor();
	int getPieceValue();
};
