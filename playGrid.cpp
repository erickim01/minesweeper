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

void PlayGrid::seedGrid(std::pair<int, int> userCoord) {
	//Create Registry and a shuffled copy of registry
	setGridList();
	std::vector<std::pair<int, int>> listCopy = gridList;
	auto rng = std::default_random_engine{};
	std::shuffle(std::begin(listCopy), std::end(listCopy), rng);
	for (int i = 0; i < numBombs; ++i) {
		if ((userCoord.first != listCopy[i].first) && (userCoord.second != listCopy[i].second)) {
			gameGrid[listCopy[i].first][listCopy[i].second] = -1;
		}
	}	
	//DEBUG - Show Registry Contents		for (int i = 0; i < listCopy.size(); ++i) { std::cout << listCopy[i].first << ", " << listCopy[i].second << std::endl; }
}

void PlayGrid::generateGrid() {

	int fieldSize = -1;

	const int EASY = 8;
	const int NORMAL = 12;
	const int HARD = 24;
	/*
	if (difficulty == 0) {				//Not convinced this is long enough to warrant a switch statment
		fieldSize = EASY;
		gameGrid.resize(EASY);
	}
	else if (difficulty == 1) {
		fieldSize = NORMAL;
		gameGrid.resize(NORMAL);
	}
	else if (difficulty == 2) {
		fieldSize = HARD;
		gameGrid.resize(HARD);
	}
	*/
	switch (difficulty) {
		case 0:
			fieldSize = EASY;
			gameGrid.resize(EASY);
			break;
		case 1:
			fieldSize = NORMAL;
			gameGrid.resize(NORMAL);
			break;
		case 2:
			fieldSize = HARD;
			gameGrid.resize(HARD);
			break;
		default:
			std::cout << "DEBUG PlayGrid::generateGrid() - Failed to recognize difficulty level.\n";
			break;
	}


	setGridSize(fieldSize);
	
	for (int i = 0; i < fieldSize; ++i) { //Initalize sixteen vectors of size 16 and add each to each to playField.
		std::vector<int> newVect(fieldSize);
		gameGrid[i] = newVect;
	}
	/*
	//DEBUG: DISPLAY VECTOR CONTENTS
	for (int i = 0; i < gameGrid.size(); ++i) {
		for (int j = 0; j < gameGrid.size(); ++j) {
			std::cout << gameGrid[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/
}