#include <iostream>
#include <algorithm> //	<std::ranges::shuffle>
#include <random>	//	<auto rng = std::default_random_engine {};>
#include <array>
#include <unordered_set>		//	Faster seedGrid() lookups
#include "PlayGrid.h"

/////	saveGame() function - Likely to move these #includes to a parent class.
#include <filesystem>
#include <fstream>
#include <string>
namespace fs = std::filesystem;
////	End of saveGame() & loadGame() Includes.

template<>
struct std::hash<std::pair<int, int>> {
	std::size_t operator()(const std::pair<int, int>& p) const noexcept {
		// Simple but decent hash combine (many variations exist)
		std::size_t h1 = std::hash<int>{}(p.first);
		std::size_t h2 = std::hash<int>{}(p.second);
		return h1 ^ (h2 << 1);   // or h1 + 0x9e3779b9 + (h2 << 6) + (h2 >> 2), etc.
	}
};


//PlayGrid::PlayGrid() {}

void PlayGrid::setFirstMove(bool setState) { firstMove = setState; } //	Resets firstMove flag to true at the start of a new game and once called is set to false.

void PlayGrid::setDifficulty(int difficulty) {
	this->difficulty = difficulty;
	setBombs();
}

//	Number of bombs is dependent on difficulty.
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

void PlayGrid::setFlags(int bombs) { this->flagsLeft = bombs; }

void PlayGrid::setGridSize(int gridSize) { this->gridSize = gridSize; }

void PlayGrid::setTilesRevealed(int tilesRevealed) { this->tilesRevealed = tilesRevealed; }

//	Enters every possible coordinate pair from the 2D vector into a vector of pairs
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
				if (!gameGrid[i][j].revealed && !gameGrid[i][j].flagged) { std::cout << (char)254u << " | "; } 
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
	}
}

//	Display entire Grid contents without occlusion
void PlayGrid::displayGridGameOver() {
	for (int i = 0; i < gameGrid.size() + 1; ++i) { std::cout << " | " << i; }
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
	//std::cout << "  _______________________________________________________\n\t\n";
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

//Helper that randomizes a vector of pairs.
void shuffleList(std::vector<std::pair<int, int>>& list) { std::shuffle(std::begin(list), std::end(list), std::mt19937(std::random_device{}())); }


//	Placement of bombs and processing the first click, making a free space of zeroes.
void PlayGrid::seedGrid(std::pair<int, int> userCoord) {
	setGridList();		//	Create Registry and a shuffled copy of registry
	std::vector<std::pair<int, int>> listCopy = gridList;
	shuffleList(listCopy);

	
	//	Create a list of all the neighbors of the user's first clicked square.
	//	FEATURE FEATURE FEATURE -	If desired, the magic numbers in uni_int_dist may be replaced by variables that scale with difficulty.
	auto rng = std::mt19937(std::random_device{}());
	std::vector<std::pair<int, int>> neighborCoords = getNeighbors(userCoord.first, userCoord.second, std::uniform_int_distribution(8, 8)(rng));
	std::vector<std::pair<int, int>> neighborsExtended;
	for (auto& entry : neighborCoords) {		//	FEATURE FEATURE FEATURE -	A third iteration of rdNum3(1, 2) may take place for slightly more start variety.
		std::vector<std::pair<int, int>> tempPairVector = getNeighbors(entry.first, entry.second, std::uniform_int_distribution(2, 4)(rng));
		for (auto& entry : tempPairVector) { neighborsExtended.push_back(entry); }
	}	
	for (auto& entry : neighborsExtended) { neighborCoords.push_back(entry); }
	neighborCoords.push_back(userCoord);
	std::sort(neighborCoords.begin(), neighborCoords.end());
	neighborCoords.erase(std::unique(neighborCoords.begin(), neighborCoords.end()), neighborCoords.end());		//	TODO TODO TODO - Rewrite this section as a hash table to avoid erase calls.
	
	for (auto& entry : neighborCoords) { std::cout << entry.first << ", " << entry.second << ": " << gameGrid[entry.first][entry.second].value << std::endl;; }

	//If any of the coordinates in the "safe zone" list would be made bombs, they are skipped in the next for loop.
	//This setup is such that the program will continue to attempt bomb placements until all bombs have been placed or the end of the grid is reached.
	//Check desired num of bombs and uniform_int_dist's allow enough spaces for bombs to be planted.
	int placed = 0;
	std::unordered_set<std::pair<int, int>> safeList(neighborCoords.begin(), neighborCoords.end());
	for (auto& coordinate : listCopy) {
		bool isSafe = safeList.find(coordinate) != safeList.end();
		if (!isSafe) {
			gameGrid[coordinate.first][coordinate.second].value = -1;
			if (++placed == numBombs) { break; }
		}
	}
	
	countAllNeighbors(gameGrid);

	gameGrid[userCoord.first][userCoord.second].value = 0;
	//placed -= checkNeighborsSolo(userCoord.first, userCoord.second);
	std::cout << "Bombs to be reloacted: " << numBombs - placed << std::endl;
	countAllNeighbors(gameGrid);
}



//	The initial click is not only free, but is always a zero surrounded by other zeroes.
//	DEBUG - Show Registry Contents for (int i = 0; i < listCopy.size(); ++i) { std::cout << listCopy[i].first << ", " << listCopy[i].second << std::endl; }


//	Checks every neighboring cell and returns the number of bombs neighboring a single index (a "cell") in a 2D Vector.
std::vector<std::pair<int, int>> PlayGrid::getNeighbors(const int& row, const int& cell, const int& neighborsWanted) {

	std::vector<std::pair<int, int>> neighborList;	//	A list of every neighbor the cell has.
	if ((cell != gridSize - 1) ) { neighborList.push_back(std::make_pair(row, cell + 1)); }										//	EAST - directly to the right
	if ((row != gridSize - 1) ) { neighborList.push_back(std::make_pair(row + 1, cell)); }										//	SOUTH - directly below the current cell
	if ((row != gridSize - 1) && (cell != gridSize - 1)) { neighborList.push_back(std::make_pair(row + 1, cell + 1)); }			//	SOUTH-EAST - down and right

	if ((row >= 1)) { neighborList.push_back(std::make_pair(row - 1, cell)); }													//	NORTH - directly above	
	if ((row >= 1) && (cell != gridSize - 1) ) { neighborList.push_back(std::make_pair(row - 1, cell + 1)); }						//	NORTH-EAST - up and right

	if ((cell >= 1)) { neighborList.push_back(std::make_pair(row, cell - 1)); }													//	WEST - left neighbor 
	if ((row >= 1) && (cell >= 1) ) { neighborList.push_back(std::make_pair(row - 1, cell - 1)); }								//	NORTH-WEST - up and left
	if ((row != gridSize - 1) && (cell >= 1)) { neighborList.push_back(std::make_pair(row + 1, cell - 1)); }						//	SOUTH-WEST - down and left

	/*
	int bombsRemoved = 0;
	for (auto& entry : validNeighbors) {
		if (gameGrid[entry.first][entry.second].value == -1) { 
			gameGrid[entry.first][entry.second].value = 0;
			++bombsRemoved;
		}
	}
	return bombsRemoved;
	*/

	//	The list of possible neighboring cells is randomized. As long as the number of neighbors wanted is less than the amount of possible neighbors
	shuffleList(neighborList);
	while (neighborsWanted < neighborList.size()) {	neighborList.erase(neighborList.begin()); }
	return neighborList;
}

//	Helper function to seedGrid(). Checks if a neighboring cell on a 2D grid has a -1 and then increases the current Cell's own count by one if -1 is seen.
void PlayGrid::countAllNeighbors(std::vector<std::vector<Cell>> &numVects2D) {
	for (int row = 0; row < gridSize; ++row) {
		for (int cell = 0; cell < gridSize; ++cell) {
			auto& currCell = numVects2D[row][cell].value;
			if (currCell == -1) { continue; }	//	If the current cell is already a bomb, it is skipped so as to not be overwritten.
			int count = 0;
			//	Check neighbors of the current cell
			if ((cell != gridSize - 1) && (numVects2D[row][cell + 1].value == -1)) { ++count; }									//	EAST - directly to the right
			if ((row != gridSize - 1) && (numVects2D[row + 1][cell].value == -1)){ ++count; }									//	SOUTH - directly below the current cell
			if ((row != gridSize - 1) && (cell != gridSize - 1) && (numVects2D[row + 1][cell + 1].value == -1)) { ++count; }		//	SOUTH-EAST - down and right

			if ((row >= 1) && (numVects2D[row - 1][cell].value == -1)) { ++count; }											//	NORTH - directly above	
			if ((row >= 1) && (cell != gridSize - 1) && (numVects2D[row - 1][cell + 1].value == -1)) { ++count; }					//	NORTH-EAST - up and right
			
			if ((cell >= 1) && (numVects2D[row][cell - 1].value == -1)) { ++count; }											//	WEST - left neighbor 
			if ((row >= 1) && (cell >= 1) && (numVects2D[row - 1][cell - 1].value == -1)) { ++count; }							//	NORTH-WEST - up and left
			if ((row != gridSize - 1) && (cell >= 1) && (numVects2D[row + 1][cell - 1].value == -1)) { ++count; }					//	SOUTH-WEST - down and left
			currCell = count;
		}
	}
}

/*		//	Alternate better code for countNeighbors that I don't understand
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

bool PlayGrid::clickCell(bool isRightClicked, std::pair<int, int> userCoord) {	//	RMB right-click if 1, LMB left-click if 0;
	const auto& row = userCoord.first;
	const auto& col = userCoord.second;
	Cell& currCell = gameGrid[row][col];
	if (isRightClicked) {
		if(!currCell.revealed) {	//	Flag status is changed only while the tile is unrevealed.
			if (currCell.flagged) { 
				currCell.flagged = false;
				++flagsLeft;
				return false;
			}		//	If the cell is currently flagged, removes the flag.
			else {
				currCell.flagged = true;
				--flagsLeft; 
				return false;
			}		//	Otherwise a flag is placed on the tile.
		}
	}
	else {
		//	if (currCell.flagged) { return; }
		if (gameGrid[row][col].value == -1) {
			//	Bomb was left clicked. Game over ensuses.
			//	Function to set every tile to reveal and set main state to gameover.
			std::cout << "\nYour selected cell, " << static_cast<char>(row + 65) << "-" << col + 1 << ", was a BOMB.\n";
			return true;
		}
		else if (!currCell.revealed) { revealCell(row, col, currCell.revealed, currCell.flagged); return false; }
	}
	return false;
}

//	Helper function to check neighboring cells for zeroes and cascade reveal cells.
void PlayGrid::revealCell(int row, int col, bool &isRevealed, bool &isFlagged) {
	if (!isRevealed) {
		++tilesRevealed;		//	Cell is revealed if it has not yet been revealed
		isRevealed = true;		//	Avoids double counting a cell as revealed.
		isFlagged = false;
	}
	if (gameGrid[row][col].value == 0) {		//	If a neighbor exists, and hasn't been revealed yet, add it to a list of neighbors of the current cell
		//	A "zero" tile reveals EVERY one of its neighbors; A nonzero tile only reveals itself
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

bool PlayGrid::checkDirExists(const std::string& path) { return fs::is_directory(fs::status(path)); }

//	Helper to saveGame that checks if Saves folder exists and returns save file path.
std::string getPath(const std::string& fileName) {
	fs::path saveDir = "Saves";
	fs::create_directory(saveDir);
	if (!fs::exists(saveDir)) {
		std::cerr << "Failed to find or create Saves directory!\n";
		return "";  //	Could potentially throw error instead. Currently, saves directly to solution directory.
	} 
	return(saveDir / fileName).string();
}

bool PlayGrid::saveGame(const std::string& saveName) const {
	std::ofstream saveWrite(getPath(saveName));
	if (saveWrite) {	//	As long as the file was opened the private members of the current game are written, followed by each cell value.
		saveWrite << firstMove << std::endl << difficulty << std::endl << numBombs << std::endl << tilesRevealed << std::endl << flagsLeft << std::endl;
		for (const auto& i : gameGrid) {	
			for (const auto& j : i) { saveWrite << j.value << "," << j.revealed << "," << j.flagged << " "; }
			saveWrite << std::endl;
		}
		//for (const auto& i : gridList) {}		//	For use if gridList becomes used for other features later on.
	}
	else {
		std::cerr << "Failed to open file: " << saveName << std::endl; 
		return false;
	}
	saveWrite.close();
	std::cout << "Saved game " << saveName << std::endl;
	return true;
}

//	Returns a count of the number of files in the directory. This is the highest possible number file a user can choose from.
int PlayGrid::displayFiles(const std::string& path, const std::string& pathName) {
	if (!checkDirExists(path)) { 
		std::cerr << "DISPLAY SAVES: Directory " << path << "not found.\n";
		return -1; 
	}
	int saveIndex = 0;
	for (const auto& file : fs::directory_iterator(path)) {
		std::string fileName = file.path().string();
		size_t pos = fileName.find(pathName);
		if (pos != std::string::npos) {
			fileName.erase(pos, pathName.length());
		}
		std::cout << "(" << saveIndex + 1 << ") " << fileName << std::endl;
		++saveIndex;
	}
	return saveIndex;
}

//	INPUT MUST BE CHECKED FOR IS VALID INTEGER IN RANGE OF FILE LIST.
bool PlayGrid::selectFile(const std::string& path, const int& targetIndex) {
	if (!checkDirExists(path)) {												//	This check should have been performed earlier in this program, 
		std::cerr << "DISPLAY SAVES: Directory " << path << "not found.\n";		//	however it is left in this function for redundancy.
		return 0;
	}
	int currIndex = 0;
	std::vector<std::pair<int, std::string>> fileList;
	for (const auto& file : fs::directory_iterator(path)) {
		fileList.push_back(std::make_pair(currIndex, file.path().string()));
		++currIndex;
	}
	for (auto& entry : fileList) {
		if (targetIndex == entry.first) {
			loadGame(entry.second);
			break;
		}
	}
	return 1;
}

void parseCell(Cell& cell, const std::string& cellVals) {
	std::stringstream ssSub(cellVals);
	std::string value;
	std::getline(ssSub, value, ',');
	cell.value = stoi(value);
	std::getline(ssSub, value, ',');
	cell.revealed = stoi(value);
	std::getline(ssSub, value, ',');
	cell.flagged = stoi(value);
}

void PlayGrid::loadGame(const std::string& inputFile) {
	//	READ CONTENTS OF FILE INTO EACH VARIABLE IN ORDER
	std::ifstream selectedFile(inputFile);
	if (selectedFile) {
		std::string line;
		
		//	The first five lines of the file are private member values. "firstMove" is a bool and cannot be included in the array.
		std::getline(selectedFile, line);
		firstMove = std::stoi(line);
		std::array<int*, 4> membersArray = {&difficulty, &numBombs, &tilesRevealed, &flagsLeft};
		for (int* ptr : membersArray) {
			std::getline(selectedFile, line);
			*ptr = std::stoi(line);
		}
		
		createEmptyGrid();
		std::stringstream ss(line);			//	Each cell on a row is saved as { "cell.value", "cell.revealed", "cell.flagged" }, 
		std::string currCell;				//	followed by a whitespace for the next cell. Each row is separated by a newline.
		int currRow = 0;
		while (std::getline(selectedFile, line)) {
			std::stringstream ss(line);
			std::string cellStr;
			for (int currCell = 0; currCell < gameGrid.size(); ++currCell) {
				std::getline(ss, cellStr, ' ');
				parseCell(gameGrid[currRow][currCell], cellStr);
			}
			++currRow;
		}
	}
}