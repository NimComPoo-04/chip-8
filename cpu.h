#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>
#include "mem.h"
#include "scr.h"

#define STACK_PAGE (0xf00)
#define SPRITE_BEG (0x000)

typedef struct
{
	uint8_t v[0x10];
	uint16_t i;
	uint16_t pc;
	uint8_t sp;
	uint8_t dt;
	uint8_t st;

	mem_t *mem;
	scr_t *scr;
} cpu_t ;

uint16_t cpu_fetch(cpu_t *c);
void cpu_execute(cpu_t *c, uint16_t o);
void cpu_prnt_status(cpu_t *c);

#endif
