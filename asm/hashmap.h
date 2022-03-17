#ifndef _HASHMAP_H_
#define _HASHMAP_H_

#include <stdint.h>

typedef struct
{
	const char *key;
	int key_len;
	uint16_t value;
} pair_t;

typedef struct bucket_pair_t
{
	pair_t value;
	struct bucket_pair_t *next;
} bucket_pair_t;

typedef struct hm_t
{
	int len;
	bucket_pair_t **list; // this is confusing af lol

	int(*hash)(struct hm_t *h, pair_t *p);
} hm_t;

hm_t hm_create(int len, int(*hash)(struct hm_t *h, pair_t *p));
void hm_delete(hm_t *h);

void     hm_add(hm_t *h, const char *key, int len, uint16_t value);
uint16_t hm_get(hm_t *h, const char *key, int len);
uint16_t hm_set(hm_t *h, const char *key, int len, uint16_t value);
uint16_t hm_rm(hm_t *h, const char *key, int len);

bucket_pair_t *bucket_create(pair_t p);
void bucket_destroy(bucket_pair_t *b);	// nukes the whole bucket lol

void     bucket_add_pair(bucket_pair_t *b, pair_t p);
uint16_t bucket_get_pair(bucket_pair_t *b, const char *key, int len);
uint16_t bucket_set_pair(bucket_pair_t *b, const char *key, int len, uint16_t p);
uint16_t bucket_rm_pair(bucket_pair_t *b, const char *key, int len);

void hm_print(hm_t *h);
#endif
