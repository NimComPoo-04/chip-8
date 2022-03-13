#ifndef _SCR_H_
#define _SCR_H_

#define FPS 2

typedef struct
{
	uint8_t *buffer;
	int len;
} scr_t;

scr_t scr_init(int w, int h);
void scr_clear(scr_t *s);
void scr_load_sprite(scr_t *s, uint8_t x, uint8_t y, uint8_t *dat, uint8_t len);
void scr_dinit(scr_t *c);
int scr_update(scr_t *c);

/*
 *  Keypad: 
 *
 *  1 2 3 4       1 2 3 c
 *  q w e r  -->  4 5 6 d
 *  a s d f       7 8 9 e
 *  z x c v       a 0 b f
 *
 *  the number of key pressed is register in keypad variable
 */

extern uint8_t keypad;

#endif
