#pragma once
#include <vector> //<std::vector>
#include <utility> //<std::pair> for list of grid coordinates

class PlayGrid {

public:

	void setDifficulty(int difficulty);
	void functionName();
	void displayGrid();
	void generateGrid(int difficulty);
	int seedGrid(int difficulty, std::string inVal);
	

private:
	int difficulty;
	std::vector<std::vector<int> > gameGrid;			//2D Matrix representation of every cell on the playing field.
	std::vector<std::pair<int, int>> gridCoordList;		//Registry of every possible cell in play to simplify bomb seeding.

};

