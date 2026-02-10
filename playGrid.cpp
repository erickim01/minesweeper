#include <iostream>
#include <algorithm> //<std::ranges::shuffle>
#include <random> //<auto rng = std::default_random_engine {};>
#include <ranges>
#include "PlayGrid.h"



//PlayGrid::PlayGrid() {}
void PlayGrid::setDifficulty(int difficulty) {
	this->difficulty = difficulty;
	setBombs();
}

//Number of bombs is dependent on difficulty.
void PlayGrid::setBombs() {
	const int BOMBS_EASY = 10;
	const int BOMBS_NORMAL = 143;		//DEBUG DEBUG DEBUG CHANGE BACK TO 64 IN LIVE
	const int BOMBS_HARD = 100;
	switch (difficulty) {
	case 0:
		this->numBombs = BOMBS_EASY;
		break;
	case 1:
		this->numBombs = BOMBS_NORMAL;
		break;
	case 2:
		this->numBombs = BOMBS_HARD;
		break;
	default:
		std::cout << "ERROR: INVALID DIFFICULTY IN	SWITCH setBombs()." << std::endl;
		break;
	}
}

void PlayGrid::setGridSize(int gridSize) {
	this->gridSize = gridSize;
}

//Enters every possible coordinate pair from the 2D vector into a vector of pairs
void PlayGrid::setGridList() {
	for (int i = 0; i < gameGrid.size(); ++i) {
		for (int j = 0; j < gameGrid.size(); ++j) {
			gridList.push_back({i, j});
		}
	}
}

int PlayGrid::getDifficulty() { return difficulty; }

int PlayGrid::getBombs() { return numBombs; }

int PlayGrid::getGridSize() { return gridSize; }

//void PlayGrid::getGridList() {}

//TODO TODO TODO - Modify displayGrid with a "clicked" flag so clicked cells display their int value.
void PlayGrid::displayGrid() {
	for (int i = 0; i < gameGrid.size() + 1; ++i) {
		std::cout << " | " << i;
	}
	std::cout << " |" << std::endl;

	char rowLetter = 'A';
	for (int i = 0; i < gameGrid.size(); ++i) {
		std::cout << " | " << rowLetter << " | ";
		for (int j = 0; j < gameGrid.size(); ++j) {
			if (j + 1 < 10) {
				std::cout << (char)254u << " | ";
			}
			else if (j + 1 >= 10) {
				std::cout << (char)254u << "  | ";
			}
		}
		++rowLetter;
		std::cout << std::endl;
	}
}

//DEBUG - Display entire Grid contents without occlusion
void PlayGrid::displayGridGameOver() {
	for (int i = 0; i < gameGrid.size() + 1; ++i) {
		std::cout << " | " << i;
	}
	std::cout << " |" << std::endl;

	char rowLetter = 'A';
	for (int i = 0; i < gameGrid.size(); ++i) {
		std::cout << " | " << rowLetter << " | ";
		for (int j = 0; j < gameGrid.size(); ++j) {
			if (j + 1 < 10) {
				if (gameGrid[i][j] == -1) { std::cout << (char)254u << " | "; }
				else { std::cout << gameGrid[i][j] << " | "; }
			}
			else if (j + 1 >= 10) {
				if (gameGrid[i][j] == -1) { std::cout << (char)254u << "  | "; }
				else { std::cout << gameGrid[i][j] << "  | "; }
			}
		}
		++rowLetter;
		std::cout << std::endl;
	}
	std::cout << "  _______________________________________________________\n\t\n";
}

void PlayGrid::createEmptyGrid() {
	const int EASY = 8;
	const int NORMAL = 12;
	const int HARD = 24;
	switch (difficulty) {
		case 0:
			setGridSize(EASY);
			gameGrid.resize(EASY, std::vector<int>(EASY, 0));
			break;
		case 1:
			setGridSize(NORMAL);
			gameGrid.resize(NORMAL, std::vector<int>(NORMAL, 0));
			break;
		case 2:
			setGridSize(HARD);
			gameGrid.resize(HARD, std::vector<int>(HARD, 0));
			break;
		default:
			std::cout << "DEBUG PlayGrid::generateGrid() - Failed to recognize difficulty level.\n";
			break;
	}
}

void PlayGrid::seedGrid(std::pair<int, int> userCoord) {
	setGridList();		//Create Registry and a shuffled copy of registry
	std::vector<std::pair<int, int>> listCopy = gridList;
	std::random_device rd;
	std::mt19937 rng(rd());
	std::shuffle(std::begin(listCopy), std::end(listCopy), rng);

	int placed = 0;
	for (auto& coordinate : listCopy) {
		if (coordinate != userCoord) {
			gameGrid[coordinate.first][coordinate.second] = -1;
			if (++placed == numBombs) { break; }
		}
	}
	//DEBUG - Show Registry Contents for (int i = 0; i < listCopy.size(); ++i) { std::cout << listCopy[i].first << ", " << listCopy[i].second << std::endl; }
	countNeighbors(gameGrid);
}

//Helper function to seedGrid(). Checks if a neighboring cell on a 2D grid has a -1 and then increases the current Cell's own count by one if -1 is seen.
void PlayGrid::countNeighbors(std::vector<std::vector<int>> &numVects2D) {
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			auto& currCell = numVects2D[i][j];
			if (currCell == -1) { continue; }
			int count = 0;

			//Check neighbors of cell

			/*
			+++ If the row is greater or equal to one:
---- Is the row before occupied? [i - 1][j]				NORTH
----Is the cell top right? [i - 1][j+1]					NORTH EAST
++++ If cell is greater equal to one:
----- Is the cell imm. before occ? [i][j - 1]			WEST
----- Is the cell top left? [i - 1][j - 1]				NORTH WEST
				*/
			if ((j != gridSize - 1) && (numVects2D[i][j + 1] == -1)) { ++count; }								//If the immediate next element exists and equals -1	(EAST)
			if ((i != gridSize - 1) && (numVects2D[i + 1][j] == -1)){ ++count; }								//If the immediate next row at the same element position exists and equals -1	(SOUTH)
			if ((i != gridSize - 1) && (j != gridSize - 1) && (numVects2D[i + 1][j + 1] == -1)) { ++count; }		//If the immediate next element on the immediate next row exists and equals -1	(SOUTH-EAST)

			if ((i >= 1) && (numVects2D[i - 1][j] == -1)) { ++count; }												//If the element in the row immediately above exists and equals -1	(NORTH)
			if ((i >= 1) && (j != gridSize - 1) && (numVects2D[i - 1][j + 1] == -1)) { ++count; }						//If the element one space forwards in the row immediately above exists and equals -1 (NORTH-EAST)







			currCell = count;
		}
	}
}



/*		Alternate better code for countNeighbors that I don't understand
* for (int i = 0; i < numVects2D.size(); ++i) {
		auto& row = numVects2D[i];
		for(int j = 0; j < row.size(); ++j) {
			auto& cell = row[j];
			if (cell == -1) { continue; }
			int count = 0;
			//Check neighbors of cell
			cell = count + 3;
		}
		std::cout << std::endl;
	}
*/