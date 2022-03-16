#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

void __panik(int threat_lvl, const char *file, const char *func, int line_no, const char *msg, ...)
{
	switch(threat_lvl)
	{
		case 0: fprintf(stderr, "\033[1;94m[ INFO ]\033[0m\t:: %s\t: %s\t: %d\t:- ", file, func, line_no); break;
		case 1: fprintf(stderr, "\033[1;93m[ WARNING ]\033[0m\t:: %s\t: %s\t: %d\t:- ", file, func, line_no); break;
		case 2: fprintf(stderr, "\033[1;91m[ ERROR ]\033[0m\t:: %s\t: %s\t: %d\t:- ", file, func, line_no); break;
		case 3: fprintf(stderr, "\033[1;5;41;30m[ FATAL ERROR ]\033[0m\t:: %s\t: %s\t: %d\t:- ", file, func, line_no); break;
	}

	va_list v;
	va_start(v, msg);
	vfprintf(stderr, msg, v);
	va_end(v);

	fprintf(stderr, "\n");

	if(threat_lvl == 3) abort();
}
