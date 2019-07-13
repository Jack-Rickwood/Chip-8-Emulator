#pragma once

class Chip8 {
private:
	unsigned short opcode;
	unsigned char memory[4096];
	// 0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu) (0 -511)
	// 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F) (80 - 160)
	// 0x200 - 0xFFF - Program ROM and work RAM (512 - 495)
	unsigned char V[16];
	unsigned short I;
	unsigned short pc;
	unsigned char gfx[64 * 32];
	unsigned char delay_timer;
	unsigned char sound_timer;
	unsigned short stack[16];
	unsigned short sp;
	unsigned char key[16];
public:
	void InitializeSystem();
	void LoadRom(std::string filepath);
	void EmulateCycle();
	void DrawGraphics();
	bool DrawFlag;
};
