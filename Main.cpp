#include <string>
#include <iostream>
#include <vector>

#include <algorithm>	//<std::transform> <std::remove_if>
#include <cctype>		//<std::tolower> <std::toupper>		//These handle the user input when choosing menu options.
#include <limits> //numeric_limits

#include "PlayGrid.h"

void clearConsole() {
	std::cout << "\x1b[2J\x1b[1;1H";
	//ANSI Clear Screen			-	\x1b[2J 
	//ANSI Cursor to Top Left	-	\x1b[1;1H
}

enum class GameState {
	Menu = 1,
	Active = 2,
	Quit = 3

};

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

		int difficulty;
		if (menuInput == "quit" || menuInput == "q") { //quits the game
			status = GameState::Quit;
			break;
		}
		else { //Otherwise difficulty is selected.

			//For now, only a normal difficulty field of 16x16 grid and 40 mines. Later passes difficulty as parameter to choose size.
			difficulty = 0;
			gridObject.setDifficulty(difficulty);
			gridObject.generateGrid();
			status = GameState::Active;
			clearConsole();
			std::cout << "\n\t\t\tGame Start.\n";
		}

		//////ACTIVE GAME LOOP//////
		//int coordInput;							//TODO TODO TODO coordInput w/ Try-Catch to take only coordinates
		while (static_cast<int>(status) == 2) { 
			gridObject.displayGrid();
			std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid: ";
			std::cin >> menuInput;
			std::cin.ignore();
			gridObject.seedGrid(menuInput); //The first square chosen is always free. The coordinate square is used as a random number to seed the rest of the field.

			
			//int squaresLeft = 


			bool gameOver = false; //Will need value to track how many clear squares are uncleared. When all safe squares are clicked OR a bomb is clicked, gameOver = true.
			if (gameOver) {
				status = GameState::Menu;
			}
		}
	}
	
	
	std::cout << std::endl << std::endl;
	
	bool needInput = true;
	char coordChar;
	int coordInt;
	while (needInput) {
		std::string myStr;
		std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid: ";
		std::getline(std::cin, myStr);		
		myStr.erase(std::remove_if(myStr.begin(), myStr.end(), [](unsigned char c) { return std::isspace(c);  }), myStr.end());
		// DEBUG std::cout << "Raw after getline: [" << myStr << "], len=" << myStr.length() << "\n";
		if (myStr.length() < 2 || myStr.length() > 3) {
			std::cout << "\nInvalid length! Use a single letter followed by 1 - 2 digits (e.g. A1, B2, etc.)\n";
			continue; 
		}
		//Get the first char and check if it's a valid character. On Difficulty = 1 "Normal", this is A - L.		
		char candidateChar = std::toupper(static_cast<unsigned char>(myStr.at(0)));
		if (candidateChar < 'A' || candidateChar > 'L') {
			//TODO TODO TODO "A - L" is incorrect. Range must be autospecified based on length via difficulty.
			std::cout << "\nInvalid character! Please use a valid character ranging from " << "A" << " to " << "L" << ".\n";
			continue;
		}
		//Get the second char and check if it's a valid integer, and convert to an int. On "Normal", this is 1 - 12.
		std::string numStr = myStr.substr(1);
		bool isValidNum = !numStr.empty();
		for (char c : numStr) {
			if (!std::isdigit(static_cast<unsigned char>(c))) {
				isValidNum = false;
				break;
			}
		}
		if (!isValidNum) {
			std::cout << "\nInvalid number! Use digits 1-12.\n";
			continue;
		}

		int diffSize = 12;
		int candidateInt = std::stoi(numStr);
		if (candidateInt < 1 || candidateInt > diffSize) {
			std::cout << "Number " << candidateInt << " is out of range (1-" << diffSize << ").\n";
			continue;
		}

		// Success
		coordChar = candidateChar;
		coordInt = candidateInt;
		std::cout << "Your input: " << coordChar << coordInt << std::endl;
		needInput = false;
	}
	clearConsole();
	std::cout << "\nGoodbye.\n";
	return 0;
}