#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>

typedef struct
{
	uint8_t *data;
	uint16_t len;
} mem_t;

uint8_t mem_read(mem_t *m, uint16_t indx);
void mem_write(mem_t *m, uint16_t indx, uint8_t v);

void mem_save(mem_t *m, uint16_t dest, uint8_t *src, int len);
void mem_load(mem_t *m, uint8_t *dest, uint16_t src, int len);

#endif
