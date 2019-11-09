# CHIP-8-Emulator
![alt-text](https://github.com/Jack-Rickwood/Chip-8-Emulator/blob/master/images/Pong.png "Pong")
My attempt at writing a CHIP-8 emulator in c++

## Compiling and Running
- I used MSVC 2019 to compile.
- Make sure you are building for 64 bit and `./include` is added as an include directory and `./lib` is added as a library directory.
- Make sure `SDL2.lib` and `SDL2main.lib` are added as libraries.
- Make sure you have the SDL2 development library downloaded. Put the 64 bit lib files into `./lib` and the .h files into `./include`.
- Make sure SDL2.dll is in the directory with your exe when you run it.
- Right now for debug purposes the program will not continue immediately and will wait for user input to continue (q will quit, p will print some info, and enter will continue). To disable this, simply comment these lines in the graphics opcode:
	`string response;

	getline(cin, response);

	if (response.compare("p") == 0) {

		EmulatedSystem.PrintState();

	}

	else if (response.compare("q") == 0) {

		exit(0);

	}`

Will add more detailed instructions later.

## Usage
Simply run the exe with the path to your ROM as an argument.

## References
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- http://en.wikipedia.org/wiki/CHIP-8

## Requirements
- Requires SDL2 development library which you can get from [here](https://www.libsdl.org/download-2.0.php).

## Compatibility
Works on Windows and MacOSX. Untested on Linux.