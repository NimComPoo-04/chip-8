#include <stdio.h>
#include "hashmap.h"

int main(void)
{
	hm_t h = hm_create(32, NULL);

	hm_add(&h, "abc", 3, 10);
	hm_add(&h, "mother", 6, 10);
	hm_add(&h, "no", 2, 10);

	hm_set(&h, "mother", 6, 12);
	hm_add(&h, "mother", 6, 23);
	
	hm_rm(&h, "mother", 6);

	hm_print(&h);

	hm_delete(&h);
}
