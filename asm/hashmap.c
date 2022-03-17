#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "hashmap.h"

static int default_hash(struct hm_t *h, pair_t *p)
{
	// TODO: get a real hash function here dipshit
	return p->key_len % h->len; 
}

hm_t hm_create(int len, int(*hash)(struct hm_t *h, pair_t *p))
{
	hm_t h = {0};

	h.len = len;
	h.list = calloc(sizeof(bucket_pair_t*), len);
	h.hash = hash == NULL ? &default_hash : hash;

	return h;
}

void hm_delete(hm_t *h)
{
	for(int i = 0; i < h->len; i++)
		bucket_destroy(h->list[i]);
	free(h->list);
}

void hm_add(hm_t *h, const char *key, int len, uint16_t value)
{
	pair_t p = {key, len, value};
	int loc = h->hash(h, &p);
	if(h->list[loc] != NULL)
	{
		if(h->list[loc]->value.key_len == p.key_len &&
				strncmp(h->list[loc]->value.key, p.key, p.key_len) == 0)
			WTF("KEY NOT UNIQUE");
		bucket_add_pair(h->list[loc], p);
	}
	else
		h->list[loc] = bucket_create(p);
}

uint16_t hm_get(hm_t *h, const char *key, int len)
{
	pair_t p = {key, len, 0};
	int loc = h->hash(h, &p);
	return bucket_get_pair(h->list[loc], key, len);
}

uint16_t hm_set(hm_t *h, const char *key, int len, uint16_t value)
{
	pair_t p = {key, len, 0};
	int loc = h->hash(h, &p);
	return bucket_set_pair(h->list[loc], key, len, value);
}

uint16_t hm_rm(hm_t *h, const char *key, int len)
{
	pair_t p = {key, len, 0};
	int loc = h->hash(h, &p);

	if(h->list[loc]->value.key_len == p.key_len &&
			strncmp(h->list[loc]->value.key, p.key, p.key_len) == 0)
	{
		bucket_pair_t *x = h->list[loc];
		uint16_t gg = x->value.value;
		h->list[loc] = h->list[loc]->next;
		free(x);
		return gg;
	}
	else
		return bucket_rm_pair(h->list[loc], key, len);
}

bucket_pair_t *bucket_create(pair_t p)
{
	bucket_pair_t *b = calloc(sizeof(bucket_pair_t), 1);
	b->value = p;
	b->next = NULL;
	return b;
}

void bucket_destroy(bucket_pair_t *b)
{
	if(b == NULL) return ;
	while(b != NULL)
	{
		bucket_pair_t *n = b->next;
		free(b);
		b = n;
	}
}

void bucket_add_pair(bucket_pair_t *b, pair_t p)
{
	while(b->next != NULL)
	{
		if(b->value.key_len == p.key_len &&
				strncmp(b->value.key, p.key, p.key_len) == 0)
			WTF("KEY NOT UNIQUE");
		b = b->next;
	}
	b->next = bucket_create(p);
}

uint16_t bucket_get_pair(bucket_pair_t *b, const char *key, int len)
{
	if(b == NULL) return 0;
	while(b != NULL)
	{
		if(b->value.key_len == len &&
				 strncmp(b->value.key, key, len) == 0)
			return b->value.value;
		b = b->next;
	}
	return 0;
}

uint16_t bucket_set_pair(bucket_pair_t *b, const char *key, int len, uint16_t p)
{
	if(b == NULL) return 0;
	
	while(b != NULL)
	{
		if(b->value.key_len == len &&
				strncmp(b->value.key, key, len) == 0)
		{
			uint16_t g = b->value.value;
			b->value.value = p;
			return g;
		}
		b = b->next;
	}

	return 0;
}

uint16_t bucket_rm_pair(bucket_pair_t *b, const char *key, int len)
{
	if(b == NULL) return 0;

	while(b->next != NULL)
	{
		if(b->next->value.key_len == len &&
				strncmp(b->next->value.key, key, len) == 0)
		{
			bucket_pair_t *k = b->next;
			b->next = b->next->next;
			uint16_t x = k->value.value;
			free(k);
			return x;
		}
		b = b->next;
	}

	return 0;
}

void bucket_print(bucket_pair_t *b)
{
	char dup[1024] = {0};
	while(b)
	{
		strncpy(dup, b->value.key, b->value.key_len);
		printf("[ %s | %d ] --> ", dup, b->value.value);
		b = b->next;
	}
}

void hm_print(hm_t *h)
{
	for(int i = 0; i < h->len; i++)
	{
		printf("%3d |-> ", i+1);
		bucket_print(h->list[i]);
		printf("\n");
	}
}
