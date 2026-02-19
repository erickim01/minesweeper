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
	void setFlags(int bombs);
	void setGridSize(int gridSize);
	void setTilesRevealed(int tilesRevealed);
	void setGridList();
	
	bool getFirstMove();
	int getDifficulty();
	int getBombs();
	int getFlags();
	int getGridSize();
	int getTilesRevealed();
	//void getGridList();

	void displayGrid();
	void displayGridGameOver();
	void createEmptyGrid();
	void seedGrid(std::pair<int, int> userCoord);
	bool clickCell(bool clickChoice, std::pair<int, int> userCoord);
	bool checkDirExists(const std::string& path);						//	All public functions from this point on likely to be moved to base class later.
	bool saveGame(const std::string& saveName) const;
	int displayFiles(const std::string& path, const std::string& pathName);
	bool selectFile(const std::string& path, const int& targetIndex);
	void loadGame(const std::string& selectedFile);
	

private:
	bool firstMove = false;
	int difficulty = -1;
	int numBombs = -1;
	int gridSize = -1;
	int tilesRevealed = 0;													//	Keeps a count of how many non-bomb tiles have been revealed.
	int flagsLeft = -1;
	std::vector<std::vector<Cell>> gameGrid;								//	2D Matrix representation of every cell on the playing field.
	std::vector<std::pair<int, int>> gridList;								//	A registry of every possible cell in play to simplify bomb seeding.
	void countAllNeighbors(std::vector<std::vector<Cell>>& numVects2D);
	std::vector<std::pair<int, int>> getNeighbors(const int& row, const int& cell, const int& numNeighbors);
	void revealCell(int row, int col, bool& isRevealed, bool& isFlagged);
};

//	TODO TODO TODO
/*
*	- Right clicking replaces square with flag				(DONE)
* 
*	- Right clicking open/revealed tile does nothing		(DONE)
* 
*	- Left clicking reveals tile, and if it's a bomb		(DONE)
	  immediately triggers game over.

*	- Revealing all non-bomb tiles triggers victory,		  60%
      saves to high score.

	- Right Clicking decrements flagsLeft by one.			(DONE)

	- First click is a gaurunteed zero, and random			(DONE)
	  amount of neighbors are also zeroes.

	- time counter that updates timeElapsed with each		  10%
	  click.

	- tilesClicked function that divides number of			(DONE)
	  revealed tiles *that are not bombs* by gridSize 
	  squared, and then multipled by 100%.

	- Save/Load game exports/imports.						 (DONE)

	- High score log.										 80% (NEEDS TESTING)

	//	UNINCLUDED FEATURES LIST	
	- seedGrid() - If desired, the magic numbers in uni_int_dist may be replaced by variables that scale with difficulty.

	- seedGrid() - A third iteration of rdNum3(1, 2) may take place for slightly more start variety.

	- saveGame() - A game that was already saved once tracks the name of the save, and saves to that file

	- saveGame() - Overwrite save name handling.

	- saveGame() - Handle attempts to make a save with invalid file characters.
*/

