#include "mem.h"
#include "log.h"

#include <stdio.h>

uint8_t mem_read(mem_t *m, uint16_t indx)
{
	//LOG("MEM READ: %x", indx);
	if(indx > m->len)
		ERR("READ MEM OUT OF BOUNDS | %x |", indx);
	return m->data[indx];
}

void mem_write(mem_t *m, uint16_t indx, uint8_t v)
{
	//LOG("MEM WRITE: %x", indx);
	if(indx > m->len)
		ERR("READ MEM OUT OF BOUNDS | %x |", indx);
	m->data[indx] = v;
}

void mem_save(mem_t *m, uint16_t dest, uint8_t *src, int len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		mem_write(m, i+dest, src[i]);
	}
}

void mem_load(mem_t *m, uint8_t *dest, uint16_t src, int len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		dest[i] = mem_read(m, src+i);
	}
}
