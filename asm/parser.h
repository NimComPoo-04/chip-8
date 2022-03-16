#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdint.h>
#include "lexer.h"

enum
{
	OP_NONE,
	OP_NNN,
	OP_XNN,
	OP_XYN
};

typedef struct
{
	int type;
	uint8_t opcode;
	union
	{
		uint16_t nnn;
		struct { uint8_t x; uint8_t nn; } xnn;
		struct { uint8_t x; uint8_t y; uint8_t n; } xyn;
	};
} op_t;

typedef struct
{
	op_t *ins;	// instructions
	int ins_len;

	blk_t *lable;	// lables
	int lable_len;

	uint16_t bytes_in; // how many bytes in are we

	lexer_t stream;	// stream of tokens
} parser_t;

parser_t parser_create(const char *src);
void parser_print(parser_t *p);
void parser_delete(parser_t *p);

#endif
