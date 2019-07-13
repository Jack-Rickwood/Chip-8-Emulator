#include <iostream>
#include <iomanip>
#include <bitset>
#include <list>
#include <fstream>
#include <cstring>
#include <cstddef>
//#include <process.h>
#include <ctype.h>
#include <stdio.h>
#include "MainEmu.h"

using namespace std;
Chip8 EmulatedSystem;
unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int main(int argc, char **argv) {
	// Setup Graphics Here
	// Setup Input Here

	// Initialize System
	cout << "Initializing system..." << endl;
	EmulatedSystem.InitializeSystem();

	// Load Rom
	if (argc == 1) {
		cout << "An error occured! Rom couldn't be found. Did you provide one?" << endl;
                exit(1);
	}
	else if (argc >= 2) {
		cout << "Loading the last rom provided..." << endl;
		EmulatedSystem.LoadRom(argv[argc - 1]);
	}

	// Emulation Loop
	for (;;) {
		EmulatedSystem.EmulateCycle();

		if (EmulatedSystem.DrawFlag) {
			EmulatedSystem.DrawGraphics();
		}
		// If the draw flag is set, update the screen
		// Store key press state (Press and Release)
	}

	// End
	return 0;
}

void Chip8::InitializeSystem() {
	pc      = 0x200;  // Start program counter at 0x200 (512), the memory location where we will load our rom.
	opcode  = 0;      // Reset current opcode.
	I       = 0;      // Reset index register.
	sp      = 0;      // Reset stack pointer.
	for (int i = 0; i < 2048; ++i) {
		gfx[i]    = 0;                    // Clear display.
	}
	for (int i = 0; i < 16; i++) {
		stack[i]  = 0;
		key[i]    = 0;                    // Clear stack, keypad and V registers.
		V[i]      = 0;
	}
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;                    // Clear memory.
	}
	for (int i = 0; i < 80; ++i) {
		memory[i] = chip8_fontset[i];     // Load fontset.
	}
	// Reset timers.
	cout << "System initialized successfully!" << endl;
}

void Chip8::LoadRom(string filepath) {
	// Create a file input stream, and open our rom.
	cout << "Rom at: " + filepath + " successfully found. Loading bytes into memory..." << endl;
	ifstream Rom;
	size_t size = 0;
	Rom.open(filepath, ios::in | ios::binary | ios::ate);
	char* RomData = 0;

	// Get rom length.
	Rom.seekg(0, ios::end);
	size = Rom.tellg();
	Rom.seekg(0, ios::beg);

	// Assigning rom hex values to RomData.
	RomData = new char[size];
	Rom.read(RomData, size);

	// Looping through each byte, and loading it into memory.
	for (size_t i = 0; i < size; i++) {
		memory[i + 512] = RomData[i];
	}
	cout << "Rom successfully loaded into memory at location 512 to location " << size + 512 << endl;
}

void Chip8::EmulateCycle() {
	opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000) {
	case 0x2000:
		printf("Opcode 0x%X called.\n", opcode);
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
	break;

	case 0x3000:
		printf("Opcode 0x%X called.\n", opcode);
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
			pc += 4;
		}
		else {
			pc += 2;
		}
	break;

	case 0x6000:
		printf("Opcode 0x%X called.\n", opcode);
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
	break;

	case 0xA000:
		printf("Opcode 0x%X called.\n", opcode);
		I = opcode & 0x0FFF;
		pc += 2;
	break;

	case 0xD000: {
		printf("Opcode 0x%X called.\n", opcode);

		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0) {
					if (gfx[(x + xline + ((y + yline) * 64))] == 1) 
						V[0xF] = 1;                                 
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		DrawFlag = true;
		pc += 2;
	}
	break;

	case 0xF000:
		switch (opcode & 0x00FF) {
			case 0x0029:
				printf("Opcode 0x%X called.\n", opcode);
				I = V[(opcode & 0x0F00) >> 8] * 0x5;
				pc += 2;
			break;
		}
	break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		// pc += 2;
	}

	// Update timers.
}

void Chip8::DrawGraphics() {
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			if (gfx[y * 32 + x] != 0)
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
}
