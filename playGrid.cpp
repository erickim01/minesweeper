#include <iostream>
#include <algorithm> //<std::ranges::shuffle>
#include <random> //<auto rng = std::default_random_engine {};>
#include "PlayGrid.h"

//saveGame() function - Likely to move this function and these #includes to a parent class.
#include <filesystem>
#include <fstream>
#include <string>




//PlayGrid::PlayGrid() {}

void PlayGrid::setFirstMove(bool setState) { firstMove = setState; } //Resets firstMove flag to true at the start of a new game and once called is set to false.

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
	setFlags(numBombs);
}

void PlayGrid::setFlags(int bombs) {
	this->flagsLeft = bombs;
}

void PlayGrid::setGridSize(int gridSize) {
	this->gridSize = gridSize;
}

void PlayGrid::setTilesRevealed(int tilesRevealed) {
	this->tilesRevealed = tilesRevealed;
}

//Enters every possible coordinate pair from the 2D vector into a vector of pairs
void PlayGrid::setGridList() {
	for (int i = 0; i < gameGrid.size(); ++i) {
		for (int j = 0; j < gameGrid.size(); ++j) {
			gridList.push_back({i, j});
		}
	}
}

bool PlayGrid::getFirstMove() { return firstMove; }

int PlayGrid::getDifficulty() { return difficulty; }

int PlayGrid::getBombs() { return numBombs; }

int PlayGrid::getFlags() { return flagsLeft; }

int PlayGrid::getGridSize() { return gridSize; }

int PlayGrid::getTilesRevealed() { return tilesRevealed; }

//void PlayGrid::getGridList() {}

//
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
				if (!gameGrid[i][j].revealed && !gameGrid[i][j].flagged) { std::cout << (char)254u << " | "; } //Most common case first.
				else if (gameGrid[i][j].revealed) { std::cout << gameGrid[i][j].value << " | "; } //Check if has been revealed, and if so displays the value.
				else { std::cout << "P" << " | "; } //The Cell isflagged.
			}
			else if (j + 1 >= 10) {
				if (!gameGrid[i][j].revealed && !gameGrid[i][j].flagged) { std::cout << (char)254u << "  | "; }	
				else if (gameGrid[i][j].revealed) { std::cout << gameGrid[i][j].value << "  | "; }
				else { std::cout << "P" << "  | "; } 
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
				if (gameGrid[i][j].value == -1) { std::cout << "* | "; }
				else { std::cout << gameGrid[i][j].value << " | "; }
			}
			else if (j + 1 >= 10) {
				if (gameGrid[i][j].value == -1) { std::cout << "*  | "; }
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
//The initial click is not only free, but is always a zero surrounded by other zeroes.



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


bool PlayGrid::clickCell(bool isRightClicked, std::pair<int, int> userCoord) {	//RMB right-click if 1, LMB left-click if 0;
	const auto& row = userCoord.first;
	const auto& col = userCoord.second;
	Cell& currCell = gameGrid[row][col];
	if (isRightClicked) {
		if(!currCell.revealed) {													//Flag status is changed only while the tile is unrevealed.
			if (currCell.flagged) { 
				currCell.flagged = false;
				++flagsLeft;
				return false;
			}		//If the cell is currently flagged, removes the flag.
			else {
				currCell.flagged = true;
				--flagsLeft; 
				return false;
			}							//Otherwise a flag is placed on the tile.
		}
	}
	else {
		//if (currCell.flagged) { return; }
		if (gameGrid[row][col].value == -1) {
			//Bomb was left clicked. Game over ensuses.
			//Function to set every tile to reveal and set main state to gameover.
			std::cout << "\nYour selected cell, " << static_cast<char>(row + 65) << "-" << col + 1 << ", was a BOMB.\n";
			return true;
		}
		else if (!currCell.revealed) { revealCell(row, col, currCell.revealed, currCell.flagged); return false; }
	}
	return false;
}

//Helper function to check neighboring cells for zeroes and cascade reveal cells.
void PlayGrid::revealCell(int row, int col, bool &isRevealed, bool &isFlagged) {
	if (!isRevealed) {
		++tilesRevealed;		//Cell is revealed if it has not yet been revealed
		isRevealed = true;		//Avoids double counting a cell as revealed.
		isFlagged = false;
	}
	if (gameGrid[row][col].value == 0) {		//If a neighbor exists, and hasn't been revealed yet, add it to a list of neighbors of the current cell
		//A "zero" tile reveals EVERY one of its neighbors; A nonzero tile only reveals itself
		std::vector<std::pair<int, int>> neighborList;
		if ((col != gridSize - 1) && (!gameGrid[row][col + 1].revealed)) { neighborList.push_back(std::make_pair(row, col + 1)); }									//EAST - directly to the right
		if ((row != gridSize - 1) && (!gameGrid[row + 1][col].revealed)) { neighborList.push_back(std::make_pair(row + 1, col)); }									//SOUTH - directly below the current cell
		if ((row != gridSize - 1) && (col != gridSize - 1) && (!gameGrid[row + 1][col + 1].revealed)) { neighborList.push_back(std::make_pair(row + 1, col + 1)); }		//SOUTH-EAST - down and right

		if ((row >= 1) && (!gameGrid[row - 1][col].revealed)) { neighborList.push_back(std::make_pair(row - 1, col)); }												//NORTH - directly above	
		if ((row >= 1) && (col != gridSize - 1) && (!gameGrid[row - 1][col + 1].revealed)) { neighborList.push_back(std::make_pair(row - 1, col + 1)); }					//NORTH-EAST - up and right

		if ((col >= 1) && (!gameGrid[row][col - 1].revealed)) { neighborList.push_back(std::make_pair(row, col - 1)); }												// WEST - left neighbor 
		if ((row >= 1) && (col >= 1) && (!gameGrid[row - 1][col - 1].revealed)) { neighborList.push_back(std::make_pair(row - 1, col - 1)); }								// NORTH-WEST - up and left
		if ((row != gridSize - 1) && (col >= 1) && (!gameGrid[row + 1][col - 1].revealed)) { neighborList.push_back(std::make_pair(row + 1, col - 1)); }					// SOUTH-WEST - down and left

		for (int i = 0; i < neighborList.size(); ++i) {
			revealCell(neighborList[i].first, neighborList[i].second, gameGrid[neighborList[i].first][neighborList[i].second].revealed,
				gameGrid[neighborList[i].first][neighborList[i].second].flagged);
		}
	}
}


std::string getPath(const std::string& fileName) {
	namespace fs = std::filesystem;
	fs::path saveDir = "Saves";
	fs::create_directory(saveDir);
	if (!fs::exists(saveDir)) {
		std::cerr << "Failed to create Saves directory\n";
		return "";  // or throw, or return fallback path
	} 
	std::cout << "Created saves file.\n";
	return(saveDir / fileName).string();
}

//TODO TODO TODO - Check if a saveName with invalid characters can be handled.
bool PlayGrid::saveGame(const std::string& saveName) const {
	std::string fullPath = getPath(saveName);
	std::ofstream saveWrite(fullPath);
	if (saveWrite) {	//As long as the file was opened the private members of the current game are written, followed by each cell value.
		saveWrite << firstMove << std::endl << difficulty << std::endl << numBombs << std::endl << tilesRevealed << std::endl << flagsLeft << std::endl;
		for (const auto& i : gameGrid) {	
			for (const auto& j : i) { saveWrite << j.value << "," << j.revealed << "," << j.flagged << " "; }
			saveWrite << std::endl;
		}
		//for (const auto& i : gridList) {}		//For use if gridList becomes used for other features later on.
	}
	else {
		std::cerr << "Failed to open file: " << saveName << std::endl; 
		return false;
	}
	saveWrite.close();
	std::cout << "Saved game " << saveName << " .\n";
	return true;
}


std::vector<std::vector<Cell>> gameGrid;								//2D Matrix representation of every cell on the playing field.
std::vector<std::pair<int, int>> gridList;								//A registry of every possible cell in play to simplify bomb seeding.
