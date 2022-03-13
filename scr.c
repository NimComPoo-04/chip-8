#include <SDL.h>

#include "log.h"
#include "scr.h"

uint8_t keypad = 0;

static SDL_Window *win = 0;
static SDL_Renderer *ren = 0;
static SDL_Event eve = {0};

static int wid = 0;
static int hei = 0;

static int siz = 20;

static uint32_t tp = 0; 

scr_t scr_init(int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	win = SDL_CreateWindow("CHIP8",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w*siz, h*siz,
			SDL_WINDOW_SHOWN);

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	wid = w;
	hei = h;

	scr_t c = {
		.buffer = calloc(1, w*h),
		.len = w*h
	};
	
	tp = SDL_GetTicks();

	return c;
}

void scr_clear(scr_t *s)
{
	memset(s->buffer, 0, s->len);
}

int scr_load_sprite(scr_t *s, uint8_t x, uint8_t y, uint8_t *dat, uint8_t len)
{
	int collision = 0;
	for(int i = 0; i < len; i++)
	{
		uint8_t v = dat[i];
		for(int j = 7; j >= 0; j--)
		{
			uint8_t fx = (x + j) % wid;
			uint8_t fy = (y + i) % hei;

			if(s->buffer[fy * wid + fx] == 1 && (v & 1) == 1)
				collision = 1;
			s->buffer[fy * wid + fx] ^= v & 1;
			v >>= 1;
		}
	}
	return collision;
}

int scr_update(scr_t *c)
{
	while(SDL_PollEvent(&eve))
	{
		if(eve.type == SDL_QUIT) return 0;

		if(eve.type == SDL_KEYDOWN)
		{
#			define K(sc, n) case SDLK_##sc: keypad = n; break

			switch(eve.key.keysym.sym)
			{
				K( 1, 0x1 ); K( 2, 0x2 ); K( 3, 0x3 ); K( 4, 0xc );
				K( q, 0x4 ); K( w, 0x5 ); K( e, 0x6 ); K( r, 0xd );
				K( a, 0x7 ); K( s, 0x8 ); K( d, 0x9 ); K( f, 0xe );
				K( z, 0xa ); K( x, 0x0 ); K( c, 0xb ); K( v, 0xf );
			}

#			undef K
		}
		else if(eve.type == SDL_KEYUP)
		{
			keypad = 0;
		}
	}

	for(int i = 0; i < hei; i++)
	{
		for(int j = 0; j < wid; j++)
		{
			if(c->buffer[i * wid + j] == 1)
				SDL_SetRenderDrawColor(ren, 1*255/10, 7*255/10, 5*255/10, 255);
			else
				SDL_SetRenderDrawColor(ren, 9*255/10, 3*255/10, 5*255/10, 255);

			SDL_Rect bx = {j*siz, i*siz, siz, siz};
			SDL_RenderFillRect(ren, &bx);
		}
	}

	SDL_RenderPresent(ren);

	int32_t t = 1000/FPS - (SDL_GetTicks() - tp);

	if(t > 0)
		SDL_Delay(t);

	tp = SDL_GetTicks();

	return 1;
}

void scr_dinit(scr_t *c)
{
	free(c->buffer);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
