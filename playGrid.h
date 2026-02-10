#pragma once
#include <vector> //<std::vector>
#include <utility> //<std::pair> for list of grid coordinates

class PlayGrid {

public:

	//PlayGrid();
	void setDifficulty(int difficulty);
	void setBombs();
	void setGridSize(int gridSize);
	void setGridList();
	
	int getDifficulty();
	int getBombs();
	int getGridSize();
	//void getGridList();

	void displayGrid();
	void displayGridGameOver();
	void createEmptyGrid();
	void seedGrid(std::pair<int, int> userCoord);
	
	

private:
	int difficulty = -1;
	int numBombs = -1;
	int gridSize = -1;
	std::vector<std::vector<int>> gameGrid;			//2D Matrix representation of every cell on the playing field.
	std::vector<std::pair<int, int>> gridList;		//Registry of every possible cell in play to simplify bomb seeding.
	void countNeighbors(std::vector<std::vector<int>>& numVects2D);

};

