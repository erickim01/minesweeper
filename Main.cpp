#include <string>
#include <iostream>
#include <vector>
#include <algorithm>	//<std::transform> <std::remove_if>
#include <cctype>		//<std::tolower> <std::toupper>		//These handle the user input when choosing menu options.
#include "PlayGrid.h"

void clearConsole() {
	std::cout << "\x1b[2J\x1b[1;1H";		//ANSI Clear Screen		-	\x1b[2J 
											//ANSI Cursor to Top Left	-	\x1b[1;1H
}

enum class GameState {
	Menu = 1,
	Active = 2,
	Quit = 3

};

int selectDifficulty() {
	std::cout << "\nSelect Difficulty: \n> Easy \n> Normal \n> Hard \n\n> ";
	bool needInput = true;
	while (needInput) {
		std::string diffInput;
		std::getline(std::cin, diffInput);
		diffInput.erase(std::remove_if(diffInput.begin(), diffInput.end(), [](unsigned char c) { return std::isspace(c);  }), diffInput.end());
		//DEBUG std::cout << "Raw after getline: [" << diffInput << "], len=" << diffInput.length() << "\n";
		//std::string capDiffInput;
		//capDiffInput.reserve(diffInput.size());
		std::transform(diffInput.begin(), diffInput.end(), diffInput.begin(), [](unsigned char c) {return std::toupper(c); });
		if ((diffInput == "HARD") || (diffInput == "H")) {
			needInput = false;
			return 2;
		}
		else if ((diffInput == "NORMAL") || (diffInput == "NM")) {
			needInput = false;
			return 1;
		}
		else if ((diffInput == "EASY") || (diffInput == "EZ")) {
			needInput = false;
			return 0;
		}
		else {
			std::cout << "\nDifficulty not recognized. Please choose easy, normal, or hard difficulty by typing. \nSelect Difficulty > ";
		}
	}
	std::cout << std::endl;
}

//Implementation of a while loop to get valid coordinates via text input.
std::pair<int, int> getInput(int gridSize) {
	bool needInput = true;
	while (needInput) {
		std::string coordInput;
		std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid > ";
		std::getline(std::cin, coordInput);
		coordInput.erase(std::remove_if(coordInput.begin(), coordInput.end(), [](unsigned char c) { return std::isspace(c);  }), coordInput.end());
		// DEBUG std::cout << "Raw after getline: [" << myStr << "], len=" << myStr.length() << "\n";
		
		if (coordInput.length() < 2 || coordInput.length() > 3) {
			std::cout << "\nInvalid length! Use a single letter followed by 1 - 2 digits (e.g. A1, B2, etc.)\n";
			continue;
		}
		//Get the first char and check if it's a valid character. On Difficulty = 1 "Normal", this is A - L.		
		char candidateChar = std::toupper(static_cast<unsigned char>(coordInput.at(0)));
		if (candidateChar < 'A' || candidateChar > static_cast<char>(64 + gridSize)) {
			std::cout << "\nInvalid character! Please use a valid character ranging from " << "A" << " to " << static_cast<char>(64 + gridSize) << ".\n";
			continue;
		}
		//Get the second char and check if it's a valid integer, and convert to an int. On "Normal", this is 1 - 12.
		std::string numStr = coordInput.substr(1);
		if (numStr.empty() || !std::all_of(numStr.begin(), numStr.end(), ::isdigit)) {
			std::cout << "\nInvalid number! Use digits 1-" << gridSize << ".\n";
			continue;
		}
		
		int candidateInt = std::stoi(numStr);
		if (candidateInt < 1 || candidateInt > gridSize) {
			std::cout << "Your column number, " << candidateInt << " is out of range (1-" << gridSize << ")!\n";
			continue;
		}

		// These lines of code executing means the input was valid.
		char coordChar = candidateChar;		//Note: This returns as the ASCII int value starting from A = 65, B = 66, etc. Subtracting from 65 to get index.
		int coordInt = candidateInt;
		needInput = false;
		std::pair<int, int> coordsFinal = { (coordChar - 65), coordInt };
		return coordsFinal;
	}
}


std::string clickInput(bool &rightClick) { //returns "Right/Left-Click in output, modifies a boolean in main to describe behavior to PlayGrid class.
	std::string userInput;
	while (std::cout << "\nRight or Left click? ('R' / 'L'): ", std::getline(std::cin, userInput)) {
		//std::string capUserInput;
		//capUserInput.reserve(userInput.size());
		std::transform(userInput.begin(), userInput.end(), userInput.begin(), [](unsigned char c) {return std::toupper(c); });
		if (userInput == "RIGHT" || userInput == "R") { rightClick = true; return "Right-Click.\n"; }
		else if (userInput == "LEFT" || userInput == "L") { rightClick = true;return "Left-CLick.\n"; }
		else { std::cout << "Input not recognized. Please enter a click operation (Right / R, or Left / L.\n"; }
	}
}

int main() {
	GameState status = GameState::Menu;	
	std::string menuInput = "";
	PlayGrid gridObject;

	while (static_cast<int>(status) != 3) {		//Program runs in console while quit flag not raised.

		//////MENU - PLAY OR QUIT//////
		std::cout << "\t\t\t --- Minesweeper Clone --- \t\t\t" << std::endl;
		std::cout << "\n\n>Play ('p') \n>Quit ('q')\n";
		std::cin >> menuInput;
		std::cin.ignore();
		std::transform(menuInput.begin(), menuInput.end(), menuInput.begin(), [](unsigned char c) { return std::tolower(c); });

		if (menuInput == "quit" || menuInput == "q") { //quits the game
			status = GameState::Quit;
			break;
		}
		else { //Otherwise difficulty is selected.

			gridObject.setDifficulty(selectDifficulty());
			gridObject.generateGrid();
			status = GameState::Active;
			clearConsole();
			std::cout << "\n\t\t\tGame Start.\n";
		}
		////////////////////////////
		////				    ////
		////  ACTIVE GAME LOOP  ////
		////				    ////
		////////////////////////////
		while (static_cast<int>(status) == 2) { 
			gridObject.displayGrid();
			std::pair<int, int> userCoords = getInput(gridObject.getGridSize());
			bool rightClick = false;
			std::cout << "\nYour coordinates: " << static_cast<char>(userCoords.first + 65) << userCoords.second << ", " << clickInput(rightClick) << std::endl << std::endl;
			

										//Update cout to read R or L conditionally depending on input
			gridObject.seedGrid(userCoords); //The first square chosen is always free,

			//gridObject.clickCell(userCoords);

			//NEED
			// Click a square
			//Capability to flag a square as a bomb
			//
			
			
			
			//int squaresLeft = 


			bool gameOver = false; //Will need value to track how many clear squares are uncleared. When all safe squares are clicked OR a bomb is clicked, gameOver = true.
			if (gameOver) {
				clearConsole();
				std::cout << "\n\t\t\tGame Over.\n";
				gridObject.displayGridGameOver();
				status = GameState::Menu;
			}
		}
	}
	clearConsole();
	std::cout << "\nGoodbye.\n";
	return 0;
}