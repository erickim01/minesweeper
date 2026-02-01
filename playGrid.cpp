#include <iostream>
#include "PlayGrid.h"



//PlayGrid::PlayGrid() {}
void PlayGrid::setDifficulty(int difficulty) {
	this->difficulty = difficulty;

}

void PlayGrid::setBombs() {
	const int BOMBS_EASY = 10;
	const int BOMBS_NORMAL = 32;
	const int BOMBS_HARD = 100;

	//Need algorithm based on rand number to generate field

	switch (difficulty) {
	case 0:
		this->bombs = BOMBS_EASY;
		break;
	case 1:
		this->bombs = BOMBS_NORMAL;
		break;
	case 2:
		this->bombs = BOMBS_HARD;
		break;
	default:
		std::cout << "ERROR: INVALID DIFFICULTY IN	SWITCH setBombs()." << std::endl;
		break;
	}
}

void PlayGrid::setGridList() {
	for (int i = 0; i < gameGrid.size(); ++i) {
		for (int j = 0; j < gameGrid.size(); ++j) {
			gridList.push_back({i, j});
		}
	}

	/*
	for (int i = 0; i < gridList.size(); ++i) {
		std::cout << gridList[i].first << ", " << gridList[i].second << std::endl;
	}
	*/

}
int PlayGrid::getDifficulty() { return difficulty; }
int PlayGrid::getBombs() { return bombs; }
//void PlayGrid::getGridList() {

//}

void PlayGrid::displayGrid() {
	std::cout << " | 0";
	for (int i = 0; i < gameGrid.size(); ++i) {
		std::cout << " | " << i + 1;
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

int PlayGrid::seedGrid(std::string inVal) {
	

	//Need algorithm based on rand number to generate field

	
	return bombs; //Necessary?
}

void PlayGrid::generateGrid() {

	int fieldSize = -1;

	const int EASY = 9;
	const int NORMAL = 12;
	const int HARD = 24;
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
	for (int i = 0; i < fieldSize; ++i) { //Initalize sixteen vectors of size 16 and add each to each to playField.
		std::vector<int> newVect(fieldSize);
		gameGrid[i] = newVect;
	}
	setGridList();
	/*
	//DEBUG: DISPLAY VECTOR CONTENTS
	for (int i = 0; i < playField.size(); ++i) {
		for (int j = 0; j < playField.size(); ++j) {
			std::cout << playField[i][j] << " ";
		}
		std::cout << std::endl;
	}
	*/
}
