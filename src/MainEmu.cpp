#include <iostream>
#include <iomanip>
#include <bitset>
#include <list>
#include <string>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <process.h>
#include <ctype.h>
#include <stdio.h>
#include <thread>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_opengl.h"
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

unsigned char keymap[16] =
{
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_r,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_f,
	SDLK_z,
	SDLK_x,
	SDLK_c,
	SDLK_v,
};

int main(int argc, char **argv) {
	// Initialize Graphics
	cout << "Initializing graphics..." << endl;
	EmulatedSystem.InitializeGraphics();

	// Initialize System
	cout << "Initializing system..." << endl;
	EmulatedSystem.InitializeSystem();

	// Load Rom
	if (argc == 1) {
		cout << "An error occured! Rom couldn't be found. Did you provide one?" << endl;
	}
	else if (argc >= 2) {
		cout << "Loading the last rom provided..." << endl;
		EmulatedSystem.LoadRom(argv[argc - 1]);
	}

	SDL_Event e;
	while (true) {
		EmulatedSystem.EmulateCycle();

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exit(0);
				cout << "Closed emulation window.";
				system("pause>nul");
			}
			if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE)
					exit(0);
				for (int i = 0; i < 16; ++i) {
					if (e.key.keysym.sym == keymap[i]) {
						EmulatedSystem.key[i] = 0;
					}
				}
			}
			if (e.type == SDL_KEYUP) {

			}
		}

		if (EmulatedSystem.DrawFlag) {
			EmulatedSystem.DrawFlag = false;
			EmulatedSystem.DrawGraphics();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	// End
	system("pause>nul");
	return 0;
}

void Chip8::InitializeGraphics() {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Failed to initialize the graphics library (SDL2)\n";
	}
	GraphicsWindow = SDL_CreateWindow("Emulation Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_SHOWN);
	GraphicsRenderer = SDL_CreateRenderer(GraphicsWindow, -1, 0);
	SDL_RenderSetLogicalSize(GraphicsRenderer, 1024, 512);
	Texture = SDL_CreateTexture(GraphicsRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	cout << "Graphics initialized successfully!" << endl;
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
	delay_timer = 0;
	sound_timer = 0;
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
	case 0x0000:
		switch (opcode & 0x000F) {
			case 0x0000:
				printf("Opcode 0x%X called.\n", opcode);
				for (int i = 0; i < 2048; ++i) {
					gfx[i] = 0;
				}
				DrawFlag = true;
				pc += 2;
			break;

			case 0x000E:
				printf("Opcode 0x%X called.\n", opcode);
				--sp;
				pc = stack[sp];
				pc += 2;
			break;

			default:
				printf("Unknown opcode: 0x%X. Probably calling an RCA 1802 program.\n", opcode);
				pc += 2;
				// system("pause>nul");
				// exit(0);
			break;
		}
	break;

	case 0x1000:
		printf("Opcode 0x%X called.\n", opcode);
		pc = opcode & 0x0FFF;
	break;

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

	case 0x4000:
		printf("Opcode 0x%X called.\n", opcode);
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
			pc += 4;
		}
		else {
			pc += 2;
		}
	break;

	case 0x5000:
		printf("Opcode 0x%X called.\n", opcode);
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
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

	case 0x7000:
		printf("Opcode 0x%X called.\n", opcode);
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
	break;

	case 0x8000:
		switch (opcode & 0x000F) {
			case 0x0000:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
				pc += 2;
			break;

			case 0x0001:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
				pc += 2;
			break;

			case 0x0002:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
				pc += 2;
			break;

			case 0x0003:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
				pc += 2;
			break;

			case 0x0004:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
				if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
					V[0xF] = 1;
				else
					V[0xF] = 0;
				pc += 2;
			break;

			case 0x0005:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
				if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
					V[0xF] = 0;
				}
				else {
					V[0xF] = 1;
				}
				pc += 2;
			break;

			case 0x0006:
				printf("Opcode 0x%X called.\n", opcode);
				V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
				V[(opcode & 0x0F00) >> 8] >>= 1;
				pc += 2;
			break;

			case 0x0007:
				printf("Opcode 0x%X called.\n", opcode);
				if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
					V[0xF] = 0;
				else
					V[0xF] = 1;
				V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]);
				pc += 2;
			break;

			case 0x000E:
				printf("Opcode 0x%X called.\n", opcode);
				V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
				V[(opcode & 0x0F00) >> 8] <<= 1;
				pc += 2;
			break;

			default:
				printf("Unknown opcode: 0x%X\n", opcode);
				// system("pause>nul");
				// exit(0);
			break;
		}
	break;

	case 0x9000:
		printf("Opcode 0x%X called.\n", opcode);
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
			pc += 4;
		}
		else {
			pc += 2;
		}
	break;

	case 0xA000:
		printf("Opcode 0x%X called.\n", opcode);
		I = opcode & 0x0FFF;
		pc += 2;
	break;

	case 0xB000:
		printf("Opcode 0x%X called.\n", opcode);
		pc = (opcode & 0x0FFF) + V[0];
	break;

	case 0xC000:
		printf("Opcode 0x%X called.\n", opcode);
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % (0xFF + 1));
		pc += 2;
	break;

	case 0xD000: {
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;
		printf("Opcode 0x%X called. ", opcode);
		printf("x=%d ", V[(opcode & 0x0F00) >> 8]);
		printf("y=%d ", V[(opcode & 0x00F0) >> 4]);
		printf("height=%d\n", opcode & 0x000F);

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0 && (x + xline + ((y + yline) * 64)) <= 2048) {
					if (gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] == 1) {
						// cout << "COLLISION!" << endl;
						V[0xF] = 1;
					}
						
					gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] ^= 1;
				}
			}
		}
		string response;
		getline(cin, response);
		if (response.compare("p") == 0) {
			EmulatedSystem.PrintState();
		}
		else if (response.compare("q") == 0) {
			exit(0);
		}

		DrawFlag = true;
		pc += 2;
		break;
	}

	case 0xE000:
		switch (opcode & 0x00FF) {
			case 0x009E:
				printf("Opcode 0x%X called.\n", opcode);
				if (key[V[(opcode & 0x0F00) >> 8]] != 0)
					pc += 4;
				else
					pc += 2;
			break;

			case 0x00A1:
				printf("Opcode 0x%X called.\n", opcode);
				if (key[V[(opcode & 0x0F00) >> 8]] == 0)
					pc += 4;
				else
					pc += 2;
			break;

			default:
				printf("Unknown opcode: 0x%X\n", opcode);
				// system("pause>nul");
				// exit(0);
			break;
		}
	break;

	case 0xF000:
		switch (opcode & 0x00FF) {
			case 0x0007:
				printf("Opcode 0x%X called.\n", opcode);
				V[(opcode & 0x0F00) >> 8] = delay_timer;
				pc += 2;
			break;

			case 0x000A:
				printf("Opcode 0x%X called.\n", opcode);
				// LAST OPCODE TO IMPLEMENT (FOR INPUT)
				pc += 2;
			break;

			case 0x0015:
				printf("Opcode 0x%X called.\n", opcode);
				delay_timer = V[(opcode & 0x0F00) >> 8];
				pc += 2;
			break;

			case 0x0018:
				printf("Opcode 0x%X called.\n", opcode);
				sound_timer = V[(opcode & 0x0F00) >> 8];
				pc += 2;
			break;

			case 0x001E:
				printf("Opcode 0x%X called.\n", opcode);
				if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
					V[0xF] = 1;
				else
					V[0xF] = 0;
				I += V[opcode & 0x0F00 >> 8];
				pc += 2;
			break;

			case 0x0029:
				printf("Opcode 0x%X called.\n", opcode);
				I = V[(opcode & 0x0F00) >> 8] * 0x5;
				pc += 2;
			break;

			case 0x0033:
				printf("Opcode 0x%X called.\n", opcode);
				memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
				memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
				pc += 2;
			break;

			case 0x0055:
				printf("Opcode 0x%X called.\n", opcode);
				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					memory[I + i] = V[i];
				I += ((opcode & 0x0F00) >> 8) + 1;
				pc += 2;
			break;

			case 0x0065:
				printf("Opcode 0x%X called.\n", opcode);
				for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
					V[i] = memory[I + i];
				I += ((opcode & 0x0F00) >> 8) + 1;
				pc += 2;
			break;

			default:
				printf("Unknown opcode: 0x%X\n", opcode);
				// system("pause>nul");
				// exit(0);
			break;
		}
	break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		// system("pause>nul");
		// exit(0);
	}

	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0) {
		if (sound_timer == 1)
			// Sound here
		--sound_timer;
	}
}

void Chip8::DrawGraphics() {
	unsigned int pixels[64 * 32];
	for (int i = 0; i < 2048; ++i) {
		uint8_t pixel = gfx[i];
		pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
	}
	SDL_UpdateTexture(Texture, NULL, pixels, 64 * sizeof(unsigned int));
	SDL_RenderClear(GraphicsRenderer);
	SDL_RenderCopy(GraphicsRenderer, Texture, NULL, NULL);
	SDL_RenderPresent(GraphicsRenderer);
}

void Chip8::PrintState() {
	cout << "Program is at location " << pc - 512 << " in the rom." << "\n";
	printf("First instruction is 0x%X\n", memory[pc + 2] << 8 | memory[pc + 3]);
	printf("Second instruction is 0x%X\n", memory[pc + 4] << 8 | memory[pc + 5]);
	printf("Third instruction is 0x%X\n", memory[pc + 6] << 8 | memory[pc + 7]);
	for (int i = 0; i < 16; i++) {
		printf("Register V[%i] is 0x%X\n", i, V[i]);
	}
}