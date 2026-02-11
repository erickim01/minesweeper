#include <iostream>
#include <algorithm> //<std::ranges::shuffle>
#include <random> //<auto rng = std::default_random_engine {};>
#include "PlayGrid.h"



//PlayGrid::PlayGrid() {}
void PlayGrid::setDifficulty(int difficulty) {
	this->difficulty = difficulty;
	setBombs();
}

//Number of bombs is dependent on difficulty.
void PlayGrid::setBombs() {
	const int BOMBS_EASY = 10;
	const int BOMBS_NORMAL = 32;
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
	flagsLeft = this->numBombs;
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
		//std::cout << "Flags Remaining: " << "flags" << "\nTiles Revealed: " << "tilesOpen / (gridSize * gridSize)" << "%\nTime Elapsed: " << "timeElapsed" << std::endl;
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
				if (gameGrid[i][j].value == -1) { std::cout << (char)254u << " | "; }
				else { std::cout << gameGrid[i][j].value << " | "; }
			}
			else if (j + 1 >= 10) {
				if (gameGrid[i][j].value == -1) { std::cout << (char)254u << "  | "; }
				else { std::cout << gameGrid[i][j].value << "  | "; }
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
			gameGrid.resize(EASY, std::vector<Cell>(EASY));
			break;
		case 1:
			setGridSize(NORMAL);
			gameGrid.resize(NORMAL, std::vector<Cell>(NORMAL));
			break;
		case 2:
			setGridSize(HARD);
			gameGrid.resize(HARD, std::vector<Cell>(HARD));
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
			gameGrid[coordinate.first][coordinate.second].value = -1;
			if (++placed == numBombs) { break; }
		}
	}
	//DEBUG - Show Registry Contents for (int i = 0; i < listCopy.size(); ++i) { std::cout << listCopy[i].first << ", " << listCopy[i].second << std::endl; }
	countNeighbors(gameGrid);
}

//Helper function to seedGrid(). Checks if a neighboring cell on a 2D grid has a -1 and then increases the current Cell's own count by one if -1 is seen.
void PlayGrid::countNeighbors(std::vector<std::vector<Cell>> &numVects2D) {
	for (int i = 0; i < gridSize; ++i) {
		for (int j = 0; j < gridSize; ++j) {
			auto& currCell = numVects2D[i][j].value;
			if (currCell == -1) { continue; }	//If the current cell is already a bomb, it is skipped so as to not be overwritten.
			int count = 0;
			//Check neighbors of the current cell
			if ((j != gridSize - 1) && (numVects2D[i][j + 1].value == -1)) { ++count; }									//EAST - directly to the right
			if ((i != gridSize - 1) && (numVects2D[i + 1][j].value == -1)){ ++count; }									//SOUTH - directly below the current cell
			if ((i != gridSize - 1) && (j != gridSize - 1) && (numVects2D[i + 1][j + 1].value == -1)) { ++count; }		//SOUTH-EAST - down and right

			if ((i >= 1) && (numVects2D[i - 1][j].value == -1)) { ++count; }												//NORTH - directly above	
			if ((i >= 1) && (j != gridSize - 1) && (numVects2D[i - 1][j + 1].value == -1)) { ++count; }					//NORTH-EAST - up and right
			
			if ((j >= 1) && (numVects2D[i][j - 1].value == -1)) { ++count; }												// WEST - left neighbor 
			if ((i >= 1) && (j >= 1) && (numVects2D[i - 1][j - 1].value == -1)) { ++count; }								// NORTH-WEST - up and left
			if ((i != gridSize - 1) && (j >= 1) && (numVects2D[i + 1][j - 1].value == -1)) { ++count; }					// SOUTH-WEST - down and left
			currCell = count;
		}
	}
}



/*		Alternate better code for countNeighbors that I don't understand
* 
* //Direction Array / offsets
* int countNeighbors(int i, int j) {
    int count = 0;
    
    constexpr int di[] = {-1, -1, -1,  0,  0,  1,  1,  1};
    constexpr int dj[] = {-1,  0,  1, -1,  1, -1,  0,  1};
    
    for (int d = 0; d < 8; ++d) {
        int ni = i + di[d];
        int nj = j + dj[d];
        
        if (ni >= 0 && ni < gridSize &&
            nj >= 0 && nj < gridSize &&
            numVects2D[ni][nj] == -1) {
            ++count;
        }
    }
    
    return count;
}

	//"Better" for loops setup
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