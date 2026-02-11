#include <string>
#include <iostream>
#include <vector>
#include <algorithm>	//<std::transform> <std::remove_if>
#include <cctype>		//<std::tolower> <std::toupper>		//These handle the user input when choosing menu options.
#include "PlayGrid.h"

//ANSI Clear Screen		-	\x1b[2J 
//ANSI Cursor to Top Left	-	\x1b[1;1H
void clearConsole() { std::cout << "\x1b[2J\x1b[1;1H"; }

enum class GameState {
	Menu = 1,
	Active = 2,
	Quit = 3

};

//Removes whitespaces from input and capitalizes all alphabetic characters.
std::string normalizeInput(std::string &str) { 
	str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c);  }), str.end());
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {return std::toupper(c); });
	return str;
}

int selectDifficulty() {
	std::cout << "\nSelect Difficulty: \n> Easy \n> Normal \n> Hard \n\n> ";
	while (true) {
		std::string diffInput;
		std::getline(std::cin, diffInput);					//DEBUG std::cout << "Raw after getline: [" << diffInput << "], len=" << diffInput.length() << "\n";
		normalizeInput(diffInput);
		if ((diffInput == "HARD") || (diffInput == "H")) { return 2; }
		else if ((diffInput == "NORMAL") || (diffInput == "NM")) { return 1; }
		else if ((diffInput == "EASY") || (diffInput == "EZ")) { return 0; }
		else { std::cout << "\nDifficulty not recognized. Please choose easy, normal, or hard difficulty by typing. \nSelect Difficulty > "; }
	}
}

//Implementation of a while loop to get valid coordinates via text input.
std::pair<int, int> getInput(int gridSize) {
	bool needInput = true;
	while (needInput) {
		std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid > ";
		std::string coordInput;
		std::getline(std::cin, coordInput);				// DEBUG std::cout << "Raw after getline: [" << myStr << "], len=" << myStr.length() << "\n";
		normalizeInput(coordInput);
		
		if (coordInput.length() < 2 || coordInput.length() > 3) {
			std::cout << "\nInvalid length! Use a single letter followed by 1 - 2 digits (e.g. A1, B2, etc.)\n";
			continue;
		}
		//Get the first char and check if it's a valid character. On Difficulty = 1 "Normal", this is A - L.		
		if (coordInput.at(0) < 'A' || coordInput.at(0) > static_cast<char>(64 + gridSize)) {
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
		char coordChar = coordInput.at(0);		//Note: This returns as the ASCII int value starting from A = 65, B = 66, etc. Subtracting from 65 to get index.
		int coordInt = candidateInt;
		needInput = false;
		std::pair<int, int> coordsFinal = { (coordChar - 65), coordInt - 1};
		return coordsFinal;
	}
}

std::string clickInput(bool &rightClick) { //returns "Right/Left-Click in text output, modifies a boolean in main to describe behavior to PlayGrid class.
	std::string userInput;
	while (std::cout << "\nRight or Left click? ('R' / 'L'): ", std::getline(std::cin, userInput)) {
		normalizeInput(userInput);
		if (userInput == "RIGHT" || userInput == "R") { clearConsole(); rightClick = true; return "Right-Click."; }
		else if (userInput == "LEFT" || userInput == "L") { clearConsole(); rightClick = false; return "Left-CLick."; }
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
		std::transform(menuInput.begin(), menuInput.end(), menuInput.begin(), [](unsigned char c) { return std::toupper(c); });

		if (menuInput == "QUIT" || menuInput == "Q") { //Sets state to "Quit" and breaks the while loop.
			status = GameState::Quit;
			break;
		}
		else { //Otherwise difficulty is selected.
			gridObject.setDifficulty(selectDifficulty());
			gridObject.createEmptyGrid();
			gridObject.setFirstMove(true);
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
			//gridObject.displayGrid();
			gridObject.displayGrid(); 
			std::pair<int, int> userCoords = getInput(gridObject.getGridSize());
			bool rightClick = false;
			gridObject.displayGridGameOver(); //DEBUG Displays game over right now.
			std::cout << "\nYour coordinates: " << static_cast<char>(userCoords.first + 65) << (userCoords.second + 1) << ", " << clickInput(rightClick) << std::endl << std::endl;

		
			if (gridObject.getFirstMove()) {
				gridObject.seedGrid(userCoords);
				gridObject.setFirstMove(false);
			}

			gridObject.clickCell(rightClick, userCoords);

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