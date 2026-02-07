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

//Implementation of a while loop to get valid coordinates via text input.
std::pair<int, int> getInput(int gridSize) {
	bool needInput = true;
	while (needInput) {
		std::string coordInput;
		std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid: ";
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
		bool isValidNum = !numStr.empty();
		for (char c : numStr) {
			if (!std::isdigit(static_cast<unsigned char>(c))) {
				isValidNum = false;
				break;
			}
		}
		
		if (!isValidNum) {
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
		//<std::transform> with a lambda function or function pointer to handle type casting correctly
		std::transform(menuInput.begin(), menuInput.end(), menuInput.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (menuInput == "quit" || menuInput == "q") { //quits the game
			status = GameState::Quit;
			break;
		}
		else { //Otherwise difficulty is selected.

			//For now, only a normal difficulty field of 16x16 grid and 40 mines. Later passes difficulty as parameter to choose size.
			//TODO TODO TODO - Choose Difficulty function
	
			int difficulty = 1;
			gridObject.setDifficulty(difficulty);
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
			std::cout << "\nYour coordinates: " << static_cast<char>(userCoords.first + 65) << userCoords.second << std::endl << std::endl;
			gridObject.seedGrid(menuInput); //The first square chosen is always free. The coordinate square is used as a random number to seed the rest of the field.
			

			
			//int squaresLeft = 


			bool gameOver = false; //Will need value to track how many clear squares are uncleared. When all safe squares are clicked OR a bomb is clicked, gameOver = true.
			if (gameOver) {
				status = GameState::Menu;
			}
		}
	}
	clearConsole();
	std::cout << "\nGoodbye.\n";
	return 0;
}