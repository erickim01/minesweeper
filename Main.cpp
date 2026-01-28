#include <string>
#include <iostream>
#include <vector>

#include <algorithm>	// Required for std::transform
#include <cctype>		// Required for std::tolower.    These handle the user input when choosing menu options.

//#include <iomanip> //Required for std::setw		Handles formatting text titles messages and field format.

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

std::vector<std::vector<int> > playField;
void generateField(int difficulty) {
	
	int fieldSize = -1;

	const int EASY = 9;
	const int NORMAL = 12;
	const int HARD = 24;
	if (difficulty == 0) {				//Not convinced this is long enough to warrant a switch statment
		fieldSize = EASY;
		playField.resize(EASY);
	}
	else if (difficulty == 1) {
		fieldSize = NORMAL;
		playField.resize(NORMAL);
	}
	else if (difficulty == 2) {
		fieldSize = HARD;
		playField.resize(HARD);
	}
	for (int i = 0; i < fieldSize; ++i) { //Initalize sixteen vectors of size 16 and add each to each to playField.
		std::vector<int> newVect(fieldSize);
		playField[i] = newVect;
	}
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

int seedField(int difficulty, std::string inVal) {
	int bombs = -1;
	const int BOMBS_EASY = 10;
	const int BOMBS_NORMAL = 32;
	const int BOMBS_HARD = 100;

	//Need algorithm based on rand number to generate field

	switch (difficulty) {
		case 0:
			bombs = BOMBS_EASY;
			break;
		case 1:
			bombs = BOMBS_NORMAL;
			break;
		case 2:
			bombs = BOMBS_HARD;
			break;
		default:
			std::cout << "ERROR: INVALID DIFFICULTY IN	SWITCH seedField()." << std::endl;
			break;
	}

	return bombs;
}

void displayField() {
	std::cout << " | 0";
	for (int i = 0; i < playField.size(); ++i) {
		std::cout << " | " << i + 1;
	} 
	std::cout << " |" << std::endl;

	char rowLetter = 'A';
	for (int i = 0; i < playField.size(); ++i) {
		std::cout << " | " << rowLetter << " | ";
		for (int j = 0; j < playField.size(); ++j) {
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


int main() {
	GameState status = GameState::Menu;	
	std::string userInput = "";

	while (static_cast<int>(status) != 3) { //Program runs in console while quit flag not raised.

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
			generateField(1);
			status = GameState::Active;
			clearConsole();
			std::cout << "\n\t\t\tGame Start.\n";
		}

		//////ACTIVE GAME LOOP//////
		while (static_cast<int>(status) == 2) { 
			displayField();
			std::cout << "\nInpute ROW Letter and COLUMN Number using values shown on grid: ";
			std::cin >> userInput;
			int bombs = seedField(difficulty, userInput); //The first square chosen is always free. The coordinate square is used as a random number to seed the rest of the field.

			
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