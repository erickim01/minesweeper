#pragma once
#include <vector> //<std::vector>
#include <utility> //<std::pair> for list of grid coordinates



struct Cell {
	int		  value	= 0;					//if -1, cell contains a bomb; else has range 0 - 8
	bool revealed = false;					//Every Cell is initially hidden until left-clicked.
	bool flagged = false;					//Every Cell is initially unmarked until right clicked.
};

class PlayGrid {

public:

	//PlayGrid();
	void setFirstMove(bool setState);
	void setDifficulty(int difficulty);
	void setBombs();
	void setGridSize(int gridSize);
	void setGridList();
	
	bool getFirstMove();
	int getDifficulty();
	int getBombs();
	int getGridSize();
	//void getGridList();

	void displayGrid();
	void displayGridGameOver();
	void createEmptyGrid();
	void seedGrid(std::pair<int, int> userCoord);
	void clickCell(bool clickChoice, std::pair<int, int> userCoord);
	
	

private:
	bool firstMove;
	int difficulty = -1;
	int numBombs = -1;
	int gridSize = -1;
	int flagsLeft = -1;		//Set to numBombs
	int timeElapsed = 0;
	double tilesOpen = 0.00;
	std::vector<std::vector<Cell>> gameGrid;			//2D Matrix representation of every cell on the playing field.
	std::vector<std::pair<int, int>> gridList;		//Registry of every possible cell in play to simplify bomb seeding.
	void countNeighbors(std::vector<std::vector<Cell>>& numVects2D);

};

//TODO TODO TODO
/*
*	- Right clicking replaces square with flag
*	- Right clicking open/revealed tile does nothing
*	- Left clicking reveals tile, and if it's a bomb immediately triggers game over.
*	- Revealing all non-bomb tiles triggers victory, saves to high score.
	- Right Clicking decrements flagsLeft by one.
	- time counter that updates timeElapsed with each click.
	- tilesClicked function that divides number of revealed tiles *that are not bombs* by gridSize squared, and then multipled by 100%.
	- Save/Load game exports/imports.
	- High score log.
*/

