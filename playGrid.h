#pragma once
#include <vector> //<std::vector>
#include <utility> //<std::pair> for list of grid coordinates

class PlayGrid {

public:

	//PlayGrid();
	void setDifficulty(int difficulty);
	void setBombs();
	void setGridList();
	
	int getDifficulty();
	int getBombs();
	//void getGridList();

	void displayGrid();
	void generateGrid();
	int seedGrid(std::string inVal);
	

private:
	int difficulty = -1;
	int bombs = -1;
	std::vector<std::vector<int>> gameGrid;			//2D Matrix representation of every cell on the playing field.
	std::vector<std::pair<int, int>> gridList;		//Registry of every possible cell in play to simplify bomb seeding.

};

