#include <string>
#include <fstream>		//	Savegames and highscore record keeping.
#include <iostream>
#include <iomanip>		//	<std::put_time> and high-score output formatting.
#include <vector>
#include <algorithm>	//	<std::transform> <std::remove_if>
#include <cctype>		//	<std::tolower> <std::toupper>		//These handle the user input when choosing menu options.
#include <chrono>		//	Autosaving file naming convention and time spent in a game.
#include <format>
#include <thread>		//	Sleeping the program for a couple seconds to allow some messages to be visible.
#include "PlayGrid.h"
#include "Stopwatch.h"

//	ANSI Clear Screen		-	\x1b[2J 
//	ANSI Cursor to Top Left	-	\x1b[1;1H
void clearConsole() { std::cout << "\x1b[2J\x1b[1;1H"; }

enum class GameState {
	Menu = 1,
	Active = 2,
	Gameover = 3,
	Quit = 4

};

//	Removes whitespaces from input and capitalizes all alphabetic characters.
std::string normalizeInput(std::string &str) { 
	str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c);  }), str.end());
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {return std::toupper(c); });
	return str;
}

int selectDifficulty() {
	std::cout << "\nSelect Difficulty: \n> Easy \n> Normal \n> Hard \n\n> ";
	while (true) {
		std::string diffInput;
		std::getline(std::cin, diffInput);					//	DEBUG std::cout << "Raw after getline: [" << diffInput << "], len=" << diffInput.length() << "\n";
		normalizeInput(diffInput);
		if ((diffInput == "HARD") || (diffInput == "H")) { return 2; }
		else if ((diffInput == "NORMAL") || (diffInput == "NM") || (diffInput == "N")) { return 1; }
		else if ((diffInput == "EASY") || (diffInput == "EZ") || (diffInput == "E")) { return 0; }
		else { std::cout << "\nDifficulty not recognized. Please choose easy, normal, or hard difficulty by typing. \nSelect Difficulty > "; }
	}
}

//	Implementation of a while loop to get valid coordinates via text input.
std::pair<int, int> getCoordinates(int gridSize) {
	bool needInput = true;
	while (needInput) {
		std::cout << "\nInput ROW Letter and COLUMN Number using values shown on grid > ";
		std::string coordInput;
		std::getline(std::cin, coordInput);				// DEBUG std::cout << "Raw after getline: [" << myStr << "], len=" << myStr.length() << "\n";
		normalizeInput(coordInput);

		//	Savegame and quit sequences
		if (coordInput == "SAVE" || coordInput == "S") {
			std::pair<int, int> saveSequence = { -1, 0 };
			return saveSequence;
		}
		if (coordInput == "QUIT" || coordInput == "Q") {
			std::pair<int, int> quitSequence = { -1, 1 };
			return quitSequence;
		}
		if (coordInput.length() < 2 || coordInput.length() > 3) {
			std::cout << "\nInvalid length! Use a single letter followed by 1 - 2 digits (e.g. A1, B2, etc.)\n";
			continue;
		}
		//	Get the first char and check if it's a valid character. On Difficulty = 1 "Normal", this is A - L.		
		if (coordInput.at(0) < 'A' || coordInput.at(0) > static_cast<char>(64 + gridSize)) {
			std::cout << "\nInvalid character! Please use a valid character ranging from " << "A" << " to " << static_cast<char>(64 + gridSize) << ".\n";
			continue;
		}
		//	Get the second char and check if it's a valid integer, and convert to an int. On "Normal", this is 1 - 12.
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

		//	These lines of code executing means the input was valid.
		char coordChar = coordInput.at(0);		//	Note: This returns as the ASCII int value starting from A = 65, B = 66, etc. Subtracting from 65 to get index.
		int coordInt = candidateInt;
		needInput = false;
		std::pair<int, int> coordsFinal = { (coordChar - 65), coordInt - 1};
		return coordsFinal;
	}
}

std::string clickInput(bool &rightClick) { //	returns "Right/Left-Click in text output, modifies a boolean in main to describe behavior to PlayGrid class.
	std::string userInput;
	while (std::cout << "\nRight or Left click? ('R' / 'L'): ", std::getline(std::cin, userInput)) {
		normalizeInput(userInput);
		if (userInput == "RIGHT" || userInput == "R") { clearConsole(); rightClick = true; return "Right-Click."; }
		else if (userInput == "LEFT" || userInput == "L") { clearConsole(); rightClick = false; return "Left-CLick."; }
		else { std::cout << "Input not recognized. Please enter a click operation (Right / R, or Left / L.\n"; }
	}
}

int getValidInt(int maxVal) {
	int inVal = 0;
	while (true) {
		std::cout << "\nChoose save file by number (" << 1 << "-" << maxVal << "): ";
		if (!(std::cin >> inVal)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cerr << "Invalid input: please enter an integer.\n";
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		if (inVal < 1 || inVal > maxVal) {
			std::cerr << "Your value is out of range of zero and the highest possible file number. \n";
			continue;
		}
		return inVal;
	}
}


//	TODO TODO TODO - Fix issue of gridObject already loaded when restarting new game in same instance.
int main() {
	GameState status = GameState::Menu;	
	std::string menuInput = "";
	PlayGrid gridObject;
	Stopwatch timeKeeperObject;
	bool gameOver = false;
	while (static_cast<int>(status) != 4) {		//	Program runs in console while quit flag not raised.
		///////////////////////////////
		////// MAIN MENU OPTIONS //////
		///////////////////////////////
		std::cout << "\t\t  --- Minesweeper Clone ---\t\t" << std::endl;
		std::cout << "\n\n>Play ('p') \n\n>Load Game ('l') \n\n>High Scores ('h'/'s') \n\n>Options ('o') \n\n>Credits ('c') \n\n>Quit ('q')\n";	

		
		std::cin >> menuInput;
		std::cin.ignore();
		std::transform(menuInput.begin(), menuInput.end(), menuInput.begin(), [](unsigned char c) { return std::toupper(c); });

		if (menuInput == "QUIT" || menuInput == "Q") { //Sets state to "Quit" and breaks the while loop.
			status = GameState::Quit;
			break;
		}

		else if (menuInput == "LOAD" || menuInput == "L") {
			clearConsole();
			std::string saveDir = "Saves"; //Check if Save file exists
			if (!gridObject.checkDirExists(saveDir)) { std::cout << "Saves folder not found.\n"; }
			else {
				int fileIndexRange = gridObject.displayFiles(saveDir, "Saves\\");
				if (fileIndexRange != -1) {
					status = GameState::Active;
					gridObject.resetObject();
					gridObject.selectFile(saveDir, getValidInt(fileIndexRange) - 1);	//Needs to return a bool to check if save was invalid.
					clearConsole();
					std::cout << "\n\t\t\tGame Loaded.\n";
				}
			}
		}

		else if (menuInput == "HIGHSCORES" || menuInput == "H" || menuInput == "SCORES" || menuInput == "S" || menuInput == "HIGH-SCORES") {
			clearConsole();
			std::cout << "\n\t\t--- High Scores ---\t\t\n\n";
			std::cout << std::left << std::setw(12) << "Difficulty" << std::setw(18) << "Time" << std::setw(18) << "Tiles Revealed" << "Date\n" << std::string(60, ',') << "\n";

			std::ifstream scoreReader("scores.csv");
			if (scoreReader) {
				std::string line;
				while (std::getline(scoreReader, line)) {
					std::vector<std::string> scoreValueStr(4);
					std::stringstream ss(line);
					std::string subText;
					for (int i = 0; i < scoreValueStr.size(); ++i) {
						std::getline(ss, subText, ',');
						scoreValueStr.at(i) = subText;
					}

					//	Switch logic to convert data to sensible input for user based on difficulty settings at time of writing.
					int totalTiles = 0;	//	WARNING - This value is hardcoded and will not update if difficulty settings are tweaked in PlayGrid class!
					switch (stoi(scoreValueStr.at(0))) {
						case 0:
							scoreValueStr.at(0) = "Easy";
							totalTiles = 64 - 10;
							break;
						case 1:
							scoreValueStr.at(0) = "Normal";
							totalTiles = 144 - 32;
							break;
						case 2:
							scoreValueStr.at(0) = "Hard";
							totalTiles = 576 - 100;
							break;
					/*******case 3:
							scoreValueStr.at(0) = "Custom";
							totalTiles = (gridSize * gridSize) - numBombs;		//	TODO - Implement these variables.
							break;
					*******/
						default:
							scoreValueStr.at(0) = "ERROR";
							totalTiles = -1;
							break;
					}
					double percentVal = (static_cast<double>(stoi(scoreValueStr.at(2))) / static_cast<double>(totalTiles)) * 100;
					std::string percentStr = std::to_string(static_cast<int>(percentVal));

					//	Final outputs on the Score sheet. If all empty tiles were revealed that output is simply a "Won".
					if (static_cast<int>(stoi(scoreValueStr.at(2))) != totalTiles) {
						std::cout << std::left
							<< std::setw(12) << scoreValueStr.at(0)
							<< std::setw(18) << scoreValueStr.at(1)
							<< std::setw(18) << (scoreValueStr.at(2) + " of " + std::to_string(totalTiles) + " (" + percentStr + "%)")
							<< scoreValueStr.at(3) << "\n";
					}
					else {
						std::cout << std::left
							<< std::setw(12) << scoreValueStr.at(0)
							<< std::setw(18) << scoreValueStr.at(1)
							<< std::setw(18) << ("Won")
							<< scoreValueStr.at(3) << "\n";
					}

					
				}
				std::cout << "\nPress Enter to Continue...";
				std::cin.get();
				std::cin.ignore();
				std::cout << std::endl << std::endl;
			}
		}

		else if (menuInput == "OPTIONS" || menuInput == "O") {	//	FEATURE FEATURE FEATURE - Add options for tweak diff settings?
			clearConsole();
			std::cout << "\nOptions menu and custom difficulty settings coming soon.\n\n";
		}

		else { //Otherwise difficulty is selected.
			gridObject.resetObject();
			gridObject.setDifficulty(selectDifficulty());
			gridObject.setBombs();
			gridObject.createEmptyGrid();
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
			//Calculation of stats to display along with grid.
			static int nonBombTiles = (gridObject.getGridSize() * gridObject.getGridSize()) - gridObject.getBombs();
			float percentDone = (static_cast <float>(gridObject.getTilesRevealed()) / static_cast <float>(nonBombTiles)) * 100;
			gridObject.displayGrid();
			
			//////Game stats UI outputs//////
			//std::cout << "\nTime Elapsed: 00:00\n";
			std::cout << "\nTime Elapsed: " << timeKeeperObject.timeIs(timeKeeperObject.elapsed()) << std::endl;
			std::cout << "Tiles Revealed: " << gridObject.getTilesRevealed() << " of " << nonBombTiles;
			std::cout << " (" << std::fixed << std::setprecision(2) << percentDone << "%)";
			std::cout << "\nFlags Left : " << gridObject.getFlags() << std::endl;
			std::pair<int, int> userCoords = getCoordinates(gridObject.getGridSize());
			timeKeeperObject.start();

			//SAVE - QUIT CONDITIONALS
							
			if (userCoords.first == -1) {	//if userCoordinates returned a -1 in the first slot, a save or quit sequence has been triggered.
				clearConsole();	
				if (userCoords.second == 0) {
					std::cout << "Enter Save Name: ";
					std::string saveStr;
					std::getline(std::cin, saveStr);
					bool isSaved = gridObject.saveGame(saveStr + ".csv");
					if (isSaved) { std::cout << "Game saved successfully...\n"; }
					else { std::cout << "Failed to save game.\n"; }
					std::this_thread::sleep_for(std::chrono::seconds(3));
					continue;
				}
				else {
					if (!gridObject.getFirstMove()) {	//Autosaves occur only if at least one move has been made (i.e. bombs have been planted).
						std::cout << "Autosaving.\n";
						auto timeIs = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
						std::string autoSave = "autosave.";
						gridObject.saveGame(autoSave + std::format("{:%Y%m%d-%H%M%S}", timeIs) + ".csv");
					}
					status = GameState::Quit; 
					break;
				}
			}

			bool rightClick = false;
			std::cout << "\nYour coordinates: " << static_cast<char>(userCoords.first + 65) << (userCoords.second + 1)
				<< ", " << clickInput(rightClick) << std::endl << std::endl;

			//The grid is seeded with bombs only when the first square is revealed with a left-click.
			if (gridObject.getFirstMove() && !rightClick) {
				gridObject.seedGrid(userCoords);
				gridObject.setFirstMove(false);
			}
			clearConsole(); 
			gameOver = gridObject.clickCell(rightClick, userCoords);
			if (gridObject.getTilesRevealed() == (gridObject.getGridSize() * gridObject.getGridSize()) - gridObject.getBombs()) { 
				std::cout << "\nWinner! All open spaces have been discovered.\n";
				gameOver = true; 
			}
			if (gameOver) {
				std::cout << "\n\t\t----Game Over----\n";		//FEATURE FEATURE FEATURE - number of tabs scales with difficulty for nicer placement.
				gridObject.displayGridGameOver();
				status = GameState::Menu;

				//	Write results of game to scores.csv file.
				auto dateIs = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
				std::string currDate = std::format("{:%d-%m-%y}", dateIs);
				std::ofstream scoresWrite("scores.csv", std::ios::app);
				if (scoresWrite) { scoresWrite << gridObject.getDifficulty() << ',' << timeKeeperObject.timeIs(timeKeeperObject.elapsed()) << ',' << gridObject.getTilesRevealed() << ',' << currDate << "\n"; }
				else { std::cerr << "Warning, failed to open scores.csv to save your highscore!\n"; }
				scoresWrite.close();

				std::cout << "\nPress Enter to Continue. ";
				std::cin.get();
				std::cin.ignore();
				clearConsole();
			}
		}
	}
	if (menuInput == "CREDITS" || menuInput == "C") {
		clearConsole();
		std::cout << "\nPlanned,designed and dvloped (sic) by Eric Kim over the course of 40 hours.\n\n";
		std::cout << "\nCopyright (c) 2026 Eric Kim. See License for more details.\n\n";
	}
	std::cout << "Goodbye.\n";
	return 0;
}

/*

struct GameStats {
    std::chrono::steady_clock::time_point startTime{std::chrono::steady_clock::now()};

    // Call this whenever you want the current elapsed time
    long long elapsedSeconds() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    }

    // Optional: formatted mm:ss string for easy printing
    std::string elapsedMMSS() const {
        auto sec = elapsedSeconds();
        auto min = sec / 60;
        auto s   = sec % 60;
        return std::to_string(min) + ":" + (s < 10 ? "0" : "") + std::to_string(s);
    }
};

*/