#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "cpu.h"

uint8_t cpu_io_block = 0;
static uint8_t cpu_block_register = 0; // TODO: fix this hack plz

uint16_t cpu_fetch(cpu_t *c)
{
	if(c->dt != 0) c->dt--;
	if(c->st != 0) c->st--;

	if(cpu_io_block) return 0;
	/*TODO: Fix this ambigious problem 'cause its kinda stupid ngl*/

#ifdef _CHIP_BIGENDIAN
	uint16_t f = (mem_read(c->mem, c->pc) << 8) | mem_read(c->mem, c->pc + 1);
#else
	uint16_t f = (mem_read(c->mem, c->pc + 1) << 8) | mem_read(c->mem, c->pc);
#endif

	//LOG("%4x : %4x", c->pc, f);

	c->pc += 2;

	return f;
}

/* TODO: find  a better way to do this, I am really stupid not gonna lie*/
static void bincodeddec(uint8_t x, uint8_t *a, uint8_t *b, uint8_t *c)
{
	*a = (x/1%10);
	*b = (x/10%10);
	*c = (x/100%10);
}

void cpu_execute(cpu_t *c, uint16_t o)
{
#	define NNN (o & 0x0FFF)
#	define  NN (o & 0x00FF)
#	define   N (o & 0x000F)

#	define X (o >> 8 & 0xF)
#	define Y (o >> 4 & 0xF)

	if(cpu_io_block)
	{
		if(keypad)
		{
			c->v[cpu_block_register] = keypad;
			cpu_io_block = 0;
		}
		return;
	}

	switch(o >> 12 & 0xF)
	{
		case 0x0: switch(o)
			  {
				  case 0x00E0: scr_clear(c->scr); break;
				  case 0x00EE: {
						       if(c->sp+2+STACK_PAGE > c->mem->len)
							       ERR("CORRUPTED STACK");
						       c->sp+=2;
						       uint16_t f = (mem_read(c->mem, c->sp - 1 + STACK_PAGE ) << 8) |
							       mem_read(c->mem, c->sp + STACK_PAGE );
						       c->pc = f;
					       } 
					       break;
				  default:
					       goto UKOP;
			  }
			  break;

		case 0x1: c->pc = NNN; break;
		case 0x2: {
				  mem_write(c->mem, c->sp-1+STACK_PAGE , c->pc >> 8 & 0xFF);
				  mem_write(c->mem, c->sp+STACK_PAGE , c->pc & 0xFF);
				  c->sp -= 2;
				  c->pc = NNN;
			  }
			  break;

		case 0x3: if(c->v[X] == NN) c->pc += 2; break;
		case 0x4: if(c->v[X] != NN) c->pc += 2; break;
		case 0x5: if(c->v[X] == c->v[Y] && N == 0) c->pc += 2; break;

		case 0x6: c->v[X] = NN; break;
		case 0x7: c->v[X] += NN; break;

		case 0x8: switch(N)
			  {
				  case 0x0: c->v[X] = c->v[Y]; break;
				  case 0x1: c->v[X] |= c->v[Y]; break;
				  case 0x2: c->v[X] &= c->v[Y]; break;
				  case 0x3: c->v[X] ^= c->v[Y]; break;

				  case 0x4: c->v[0xf] = c->v[X] + c->v[Y] > 0xFF ? 1 : 0;
					    c->v[X] += c->v[Y];
					    break;

				  case 0x5: c->v[0xf] = c->v[X] < c->v[Y] ? 0 : 1;
					    c->v[X] -= c->v[Y];
					    break;

				  case 0x6: c->v[0xf] = c->v[Y] & 1; 
					    c->v[X] = c->v[Y] >> 1;
					    break;

				  case 0x7: c->v[0xf] = c->v[X] > c->v[Y] ? 0 : 1;
					    c->v[X] = c->v[Y] - c->v[X];
					    break;

				  case 0xE: c->v[0xf] = c->v[Y] & 0xA0; 
					    c->v[X] = c->v[Y] << 1;
					    break;
				  default:
					     goto UKOP;
			  }
			  break;

		case 0x9: if(c->v[X] != c->v[Y] && N == 0) c->pc += 2; break;

		case 0xA: c->i = NNN; break;
		case 0xB: c->pc = c->v[0] + NNN; break;
		case 0xC: c->v[X] = (rand()%256) & NN; break;

		case 0xD: if(scr_load_sprite(c->scr, c->v[X], c->v[Y], c->mem->data+c->i, N))
				  c->v[0xF] = 1;
			  else
				  c->v[0xF] = 0;
			  break;

		case 0xE: switch(NN)
			  {
				  case 0x9E: if(keypad == c->v[X]) c->pc += 2; break;
				  case 0xA1: if(keypad != c->v[X]) c->pc += 2; break;
				  default:
					     goto UKOP;
			  }
			  break;

		case 0xF: switch(NN)
			  {
				  case 0x07: c->v[X] = c->dt; break;
				  case 0x0A: cpu_io_block = 1;
					     cpu_block_register = X; break;
				  case 0x15: c->dt = c->v[X]; break;
				  case 0x18: c->st = c->v[X]; break;
				  case 0x1E: c->i += c->v[X]; break;
				  case 0x29: c->i = c->v[X] * 5 + SPRITE_BEG; break;
				  case 0x33: bincodeddec(c->v[X],
							     c->mem->data+c->i+0,
							     c->mem->data+c->i+1,
							     c->mem->data+c->i+2);
					     break;
				  case 0x55: mem_save(c->mem, c->i, c->v, X+1);
					     c->i += X+1;
					     break;

				  case 0x65: mem_load(c->mem, c->v, c->i, X+1);
					     c->i += X+1;
					     break;

				  default:
					     goto UKOP;
			  }
			  break;
		default:
			  UKOP:
			  ERR("UNKNOWN OPCODE: %4x", o);
	}
}

void cpu_prnt_status(cpu_t *c)
{
	if(cpu_io_block)
	{
		printf("IO BLOCK\n");
		return;
	}

	printf("V: ");
	for(int i = 0; i < 0x10; i++)
		printf("%2x\t", c->v[i]);
	printf("\n");

	printf("I : %4x\n", c->i);
	printf("PC: %4x\n", c->pc);
	printf("SP: %2x\n", c->sp);
	printf("DT: %2x\n", c->dt);
	printf("ST: %2x\n", c->st);

	printf("\n");

	// directly accessing memory to avoid error handling code
	printf("MEMORY: \n");
	printf("%4x : %2x\n", c->pc-2, c->mem->data[c->pc-2]);
	printf("%4x : %2x\n", c->pc-1, c->mem->data[c->pc-1]);
	printf("%4x : %2x <==\n", c->pc, c->mem->data[c->pc]);
	printf("%4x : %2x\n", c->pc+1, c->mem->data[c->pc+1]);
	printf("%4x : %2x\n", c->pc+2, c->mem->data[c->pc+2]);

	printf("\n");

	printf("STACK: \n");
	printf("%4x : %2x\n", c->sp-2, c->mem->data[c->sp-2+STACK_PAGE]);
	printf("%4x : %2x\n", c->sp-1, c->mem->data[c->sp-1+STACK_PAGE]);
	printf("%4x : %2x <==\n", c->sp, c->mem->data[c->sp+STACK_PAGE]);
	printf("%4x : %2x\n", c->sp+1, c->mem->data[c->sp+1+STACK_PAGE]);
	printf("%4x : %2x\n", c->sp+2, c->mem->data[c->sp+2+STACK_PAGE]);

	printf("\n");

	printf("POINTER: \n");
	printf("%4x : %2x\n", c->i-2, c->mem->data[c->i-2]);
	printf("%4x : %2x\n", c->i-1, c->mem->data[c->i-1]);
	printf("%4x : %2x <==\n", c->i, c->mem->data[c->i]);
	printf("%4x : %2x\n", c->i+1, c->mem->data[c->i+1]);
	printf("%4x : %2x\n", c->i+2, c->mem->data[c->i+2]);

	printf("\n");
}
