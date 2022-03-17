#ifndef _LEXER_H_
#define _LEXER_H_

#include "hashmap.h"

typedef struct ins_t
{
	int type;
	char *lable;
	struct ins_t *arg;
} ins_t;

typedef struct
{
	int current;
	uint16_t binloc;
} lexer_t;

#endif
