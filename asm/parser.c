#include "parser.h"

parser_t parser_create(const char *src)
{
	parser_t p = {0};
	p.stream = lexer_create(src);
}

void parser_print(parser_t *p)
{
}

void parser_delete(parser_t *p)
{
}
