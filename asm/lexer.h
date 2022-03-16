#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdint.h>

enum
{
	LEX_NONE,	// Lets make the trillion dolar mistake shall we ;)
	LEX_LABLE,
	LEX_CONST,	// constants

	LEX_BIN_NUMBER,
	LEX_OCT_NUMBER,
	LEX_DEC_NUMBER,
	LEX_HEX_NUMBER,

	LEX_INS_CLS, LEX_INS_RET, LEX_INS_JMP, LEX_INS_JSR, LEX_INS_JRL, LEX_INS_BRC,
	LEX_INS_LDR, LEX_INS_ADD, LEX_INS_MOV, LEX_INS_OR, LEX_INS_AND, LEX_INS_XOR,
	LEX_INS_ADC, LEX_INS_SUB, LEX_INS_SHL, LEX_INS_SHR, LEX_INS_RND, LEX_INS_DRAW,
	LEX_INS_KEY, LEX_INS_SPRITE, LEX_INS_BCD, LEX_INS_STM, LEX_INS_LDM,

	LEX_COND_EQ, LEX_COND_NE,
	LEX_COND_UP, LEX_COND_DOWN,
	LEX_COND_HOLD,

	LEX_V0,	LEX_V1, LEX_V2, LEX_V3, LEX_V4, LEX_V5, LEX_V6, LEX_V7,
	LEX_V8, LEX_V9, LEX_VA, LEX_VB, LEX_VC, LEX_VD, LEX_VE, LEX_VF,

	LEX_I, LEX_DT, LEX_ST
};

typedef struct
{
	int type;
	char *value;
	int len;
} blk_t;

typedef struct
{
	char *txt_stream;
	int txt_stream_len;
	blk_t *blks;
	int blks_len;
	int current;
} lexer_t ;

lexer_t lexer_create(const char *src);

blk_t lexer_blk_at(lexer_t *l, int i);
blk_t lexer_prev(lexer_t *l);
blk_t lexer_next(lexer_t *l);

void lexer_destroy(lexer_t *l);

void lexer_print(lexer_t *l);

#endif
