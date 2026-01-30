#include <string>
#include <iostream>
#include <vector>

#include <algorithm>	//<std::transform>
#include <cctype>		//<std::tolower>		//These handle the user input when choosing menu options.

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
	std::string userInput = "";
	PlayGrid gridObject;

	while (static_cast<int>(status) != 3) {		//Program runs in console while quit flag not raised.

		//////MENU - PLAY OR QUIT//////
		std::cout << "\t\t\t --- Minesweeper Clone --- \t\t\t" << std::endl;
		std::cout << "\n\n>Play ('p') \n>Quit ('q')\n";
		std::cin >> userInput;
		// Use std::transform with a lambda function or function pointer to handle type casting correctly
		std::transform(userInput.begin(), userInput.end(), userInput.begin(),
			[](unsigned char c) { return std::tolower(c); });

		int difficulty;
		if (userInput == "quit" || userInput == "q") { //quits the game
			status = GameState::Quit;
			break;
		}
		else { //Otherwise difficulty is selected.

			//For now, only a normal difficulty field of 16x16 grid and 40 mines. Later passes difficulty as parameter to choose size.
			difficulty = 1;
			gridObject.setDifficulty(difficulty);
			gridObject.generateGrid(1);
			status = GameState::Active;
			clearConsole();
			std::cout << "\n\t\t\tGame Start.\n";
		}

		//////ACTIVE GAME LOOP//////
		while (static_cast<int>(status) == 2) { 
			gridObject.displayGrid();
			std::cout << "\nInpute ROW Letter and COLUMN Number using values shown on grid: ";
			std::cin >> userInput;
			int bombs = gridObject.seedGrid(difficulty, userInput); //The first square chosen is always free. The coordinate square is used as a random number to seed the rest of the field.

			
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