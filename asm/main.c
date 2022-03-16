#include <stdio.h>
#include "lexer.h"

const char *src = 
"	cls	\n"
":a		\n"
"	jsr a	\n"
"	mov i $10\n" ;

int main(void)
{
	lexer_t l = lexer_create(src);
	lexer_print(&l);
	lexer_destroy(&l);
}
