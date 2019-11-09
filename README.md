# CHIP-8-Emulator
My attempt at writing a CHIP-8 emulator in c++

## Compiling and Running
- I used MSVC 2019 to compile.
- Make sure you are building for 64 bit and `./include` is added as an include directory and `./lib` is added as a library directory.
- Make sure `SDL2.lib` and `SDL2main.lib` are added as libraries.
- Make sure you have the SDL2 development library downloaded. Put the 64 bit lib files into `./lib` and the .h files into `./include`.
- Make sure SDL2.dll is in the directory with your exe when you run it.

Will add more detailed instructions later.

## References
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- http://en.wikipedia.org/wiki/CHIP-8

## Requirements
- Requires SDL2 development library which you can get from [here](https://www.libsdl.org/download-2.0.php).

## Compatibility
Works on Windows and MacOSX. Untested on Linux.