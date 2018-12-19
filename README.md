# 15puzzle
What is 15puzzle?
- AKA Slider puzzle, Gem puzzle, Game of fifteen
- More at wikipedia.org/wiki/15_puzzle

What are the contents of this repo?
- This repo contains :
	* Source code of the 15 puzzle in pure C(oneFIVE.c)
	* Executable of the above source code(oneFIVE.exe)
	* Required (or not, depending on your choice) a sample statistics file(file0) for above executable
	* Source code of the 15 puzzle in pure C++(oneFIVE++.cpp)
	* Executable of the above source code(oneFIVE++.exe)
	* Required (or not, depending on your choice) a sample statistics file(file9) for above executable

Why are executables provided?
- The source code requires the presence of "windows.h" header file in the IDE for successful compilation. If not available, then running only the executable is recommended.

What's in statistics?
- The statistics file records
	* Number of matches played
	* Total moves made throughout game
	* Total time played throughout game
	* Minimum moves to complete a game
	* Minimum time to complete a game

Why the same program in both C and C++?
- In C++ it is possible to input and output files in binary stream, which makes the file unreadable (and not editable) when opening files through any text editor(try this for the included "file9" in the repo) which makes it protected from unwanted formatting. This feature is not available in C(or it is, I have not figured it out yet).

Anything more to note?
- I have commented most of the lines
- No mouse support
- The game shows the number of moves made and the time taken in real time
- The game screen WILL appear to flash multiple times cause the game needs to constantly update the stopwatch count (which maybe solved using multi-threading in C++ version only).

*****Features which MAYBE implemented IN DISTANT FUTURE*****
* Multi-threading in C++
* Flexible board size (3x3,5x5)
* Having the computer solve any given puzzle