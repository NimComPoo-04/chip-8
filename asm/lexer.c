#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "lexer.h"

static int recog_type(const char *c, int len)
{
	if(len == 1)
	{
		return c[0] == 'i' ? LEX_I: LEX_NONE;
	}
	else if(len == 2)
	{
		if(c[0] == 'v')
		{
			switch(c[1])
			{
				case '0': return LEX_V0; case '1': return LEX_V1;
				case '2': return LEX_V2; case '3': return LEX_V3;
				case '4': return LEX_V4; case '5': return LEX_V5;
				case '6': return LEX_V6; case '7': return LEX_V7;
				case '8': return LEX_V8; case '9': return LEX_V9;
				case 'a': return LEX_VA; case 'b': return LEX_VB;
				case 'c': return LEX_VC; case 'd': return LEX_VD;
				case 'e': return LEX_VE; case 'f': return LEX_VF;
			}
			return LEX_NONE;
		}
		else if(c[0] == 'd' && c[1] == 't')
			return LEX_DT;
		else if(c[0] == 's' && c[1] == 't')
			return LEX_ST;
		else if(c[0] == 'o' && c[1] == 'r')
			return LEX_INS_OR;
		else if(c[0] == 'e' && c[1] == 'q')
			return LEX_COND_EQ;
		else if(c[0] == 'n' && c[1] == 'e')
			return LEX_COND_NE;
		else if(c[0] == 'u' && c[1] == 'p')
			return LEX_COND_UP;
	}
	else if(len == 3)
	{
		switch(c[0])
		{
			case 'c': return LEX_INS_CLS * (c[1] == 'l' && c[2] == 's');

			case 'r': switch(c[1])
				  {
					  case 'e': return LEX_INS_RET * (c[2] == 't');
					  case 'n': return LEX_INS_RND * (c[2] == 'd');
				  }
				  return LEX_NONE;

			case 'j': switch(c[1])
				  {
					  case 'm': return LEX_INS_JMP * (c[2] == 'p');
					  case 's': return LEX_INS_JSR * (c[2] == 'r');
					  case 'r': return LEX_INS_JRL * (c[2] == 'l');
				  }
				  return LEX_NONE;

			case 'b': switch(c[1])
				  {
					  case 'r': return LEX_INS_BRC * (c[2] == 'c');
					  case 'c': return LEX_INS_BCD * (c[2] == 'd');
				  }
				  return LEX_NONE;

			case 'l': switch(c[2])
				  {
					  case 'r': return LEX_INS_LDR * (c[1] != 'd');
					  case 'm': return LEX_INS_LDM * (c[1] != 'd');
				  }
				  return LEX_NONE;

			case 'a': switch(c[1])
				  {
					  case 'd': switch(c[2])
						    {
							    case 'd': return LEX_INS_ADD;
							    case 'c': return LEX_INS_ADC;
						    }
						    break;

					  case 'n': return LEX_INS_AND * (c[2] == 'd');
				  }
				  return LEX_NONE;

			case 'm': return LEX_INS_MOV * (c[1] == 'o' && c[2] == 'v');
			case 'x': return LEX_INS_XOR * (c[1] == 'o' && c[2] == 'r');

			case 's': switch(c[1])
				  {
					  case 'u': return LEX_INS_SUB * (c[2] == 'b');
					  case 'h': switch(c[2])
						    {
							    case 'l': return LEX_INS_SHL;
							    case 'r': return LEX_INS_SHR;
						    }
						    break;

					  case 't': return LEX_INS_STM * (c[2] == 'm');
				  }
				  break;

			case 'k': return LEX_INS_KEY * (c[1] == 'e' && c[2] == 'y');
		}
	}
	else if(len > 3)
	{
		if(len == 6 && strncmp(c, "sprite", 6))
			return LEX_INS_SPRITE;
		else if(len == 4 && strncmp(c, "draw", 4))
			return LEX_INS_DRAW;
		else if(len == 4 && strncmp(c, "down", 4))
			return LEX_COND_DOWN;
		else if(len == 4 && strncmp(c, "hold", 4))
			return LEX_COND_HOLD;
	}
	return LEX_NONE;
}

char *strdup(const char *str)
{
	int len = strlen(str);
	char *dup = calloc(1, len);
	memcpy(dup, str, len);
	return dup;
}

lexer_t lexer_create(const char *src)
{
	lexer_t l = {0};

	l.txt_stream = strdup(src);
	l.txt_stream_len = strlen(src);

	int line_no = 1;
	int idx = -1;

	while(idx < l.txt_stream_len)
	{
		blk_t cur = {0, l.txt_stream + idx, 0};

		idx++;

		switch(l.txt_stream[idx])
		{
			// non tokens
			case '\0': case ' ': case '\t': continue;

			case '\n': line_no++; continue;

			case ';': while(idx < l.txt_stream_len && l.txt_stream[idx] != '\n') idx++;
				  continue;

			case '$': idx++;
				  cur.type = LEX_DEC_NUMBER;
				  cur.value = l.txt_stream+idx;
				  cur.len = idx;

				  while(idx < l.txt_stream_len)
				  {
					  switch(l.txt_stream[idx])
					  {
						  case 0: case ' ': case '\t': case '\n':
							  goto EX;

						  case '0': case '1': case '2': case '3': case '4': case '5':
						  case '6': case '7': case '8': case '9': case 'a': case 'b':
						  case 'c': case 'd': case 'e': case 'f':
							  break;

						  case 'B': cur.type = LEX_BIN_NUMBER; goto EX;
						  case 'O': cur.type = LEX_OCT_NUMBER; goto EX;
						  case 'D': cur.type = LEX_DEC_NUMBER; goto EX;
						  case 'X': cur.type = LEX_HEX_NUMBER; goto EX;

						  default:
							    WTF("UNRECOGNIZED SYMBOL IN NUMBER : %d", line_no);
					  }
					  idx++;
				  }
EX:
				  cur.len = idx - cur.len;
				  if(strchr("BODX", l.txt_stream[idx]))
					  idx++;
				  break;

			case ':': idx++;

				  cur.type = LEX_LABLE;
				  cur.value = l.txt_stream+idx;
				  cur.len = idx;

				  while(idx < l.txt_stream_len)
				  {
					  switch(l.txt_stream[idx])
					  {
						  case 0: case ' ': case '\t': case '\n':
							  goto RC;
					  }
					  idx++;
				  }
RC:
				  cur.len = idx - cur.len;
				  break;

			case '%': idx++;

				  cur.type = LEX_CONST;
				  cur.value = l.txt_stream+idx;
				  cur.len = idx;

				  while(idx < l.txt_stream_len)
				  {
					  switch(l.txt_stream[idx])
					  {
						  case 0: case ' ': case '\t': case '\n':
							  goto TV;
					  }
					  idx++;
				  }
TV:
				  cur.len = idx - cur.len;
				  break;

			default: cur.value = l.txt_stream+idx;
				 cur.len = idx;
					 while(idx < l.txt_stream_len)
					 {
						 switch(l.txt_stream[idx])
						 { 
							 case 0: case ' ': case '\t': case '\n':
							 case ';': case ':': case '%': case '$':
								 goto YB;
						 }
						 idx++;
					 }
YB:
				 cur.len = idx - cur.len;
				 cur.type = recog_type(cur.value, cur.len);
		}

		if(l.blks_len % 16 == 0)
			l.blks = realloc(l.blks, sizeof(blk_t) * (l.blks_len+16));

		l.blks[l.blks_len++] = cur;
	}

	return l;
}

blk_t lexer_blk_at(lexer_t *l, int i)
{
	if(i < 0 || i > l->blks_len)
	{
		ERR("OUT OF BOUNDS INDEXING : %d", i);
		return (blk_t){0};
	}

	return l->blks[i];
}

blk_t lexer_prev(lexer_t *l)
{
	l->current--;
	if(l->current < 0)
	{
		BOI("BEG OF BLK STREAM");
		l->current = l->blks_len;
	}
	return l->blks[l->current];
}

blk_t lexer_next(lexer_t *l)
{
	if(l->current >= l->blks_len)
	{
		BOI("END OF BLK STREAM");
		l->current = 0;
	}
	return l->blks[l->current++];
}

void lexer_destroy(lexer_t *l)
{
	free(l->txt_stream);
	free(l->blks);
}

void lexer_print(lexer_t *l)
{
	for(int i = 0; i < l->blks_len; i++)
	{
		char c = l->blks[i].value[l->blks[i].len];
		l->blks[i].value[l->blks[i].len] = 0;

		printf("[%s | %d]\n", l->blks[i].value, l->blks[i].type);
		l->blks[i].value[l->blks[i].len] = c;
	}
}
