#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "cpu.h"
#include "mem.h"
#include "scr.h"

uint16_t program[] =
{
	0xe000,  // clear screen
	0x0a22,  // jump to subroutine 'cause i can lol
	0x1622,  // jump to subroutine 'cause i can lol
	0x2222,  // jump to subroutine 'cause i can lol
	0x0812,  // jump to starting address

	0x0a60,  // store a on v0
	0x0061,  // store 0 on v1
	0x0062,  // store 0 on v2
	0x29f0,  // load sprite on i register
	0x25D1,  // draw sprite at v1,v2 
	0xee00,  // return back don't mess about lol

	0x0b60,  // store b on v0
	0x0861,  // store 8 on v1
	0x0062,  // store 0 on v2
	0x29f0,  // load sprite on i register
	0x25D1,  // draw sprite at v1,v2 
	0xee00,  // return back don't mess about lol

	0x0c60,  // store b on v0
	0x1061,  // store 16 on v1
	0x0062,  // store 0 on v2
	0x29f0,  // load sprite on i register
	0x25D1,  // draw sprite at v1,v2 
	0xee00,  // return back don't mess about lol
};

// these fonts are basically numbers lol
uint8_t sprites[] = 
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

int load_program_from_file(const char *path, uint8_t *mem)
{
	FILE *f = fopen(path, "rb");

	if(f == NULL) return 1;

	fseek(f, 0, SEEK_END);
	int g = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(mem, 1, g, f);

	fclose(f);

	return 0;
}

int main(int argc, char *argv[])
{
	mem_t ram = {
		.data = calloc(1, 0x1000),
		.len = 0x1000 
	}; 

	if(argc >= 2)
		load_program_from_file(argv[1], ram.data + 0x200);
	else
		mem_save(&ram, 0x200, (uint8_t*)program, sizeof program);

	mem_save(&ram, SPRITE_BEG, sprites, sizeof sprites);

	scr_t scr = scr_init(0x3f, 0x1f);

	cpu_t c = {
		.v = {0},
		.i = 0,
		.pc = 0x200,
		.sp = 0xff, // cause why not ;-)
		.dt = 0,
		.st = 0,

		.mem = &ram,
		.scr = &scr
	};

	int running = 1;

	while(running)
	{
		running = scr_update(&scr);

		//cpu_prnt_status(&c);
		uint16_t op = cpu_fetch(&c);
		cpu_execute(&c, op);
	}

	scr_dinit(&scr);
}
