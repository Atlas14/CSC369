#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

static unsigned long counter;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	int evict = 0;
	// keep tracking the oldest timestamp;
	unsigned long oldest = counter;
	int i;
	
	// find the frame with the oldest timestamp (i.e. the samllest timestamp) 
	for (i = 0; i < memsize; i++) {

		if (coremap[i].pte-> timestamp < oldest) {
			oldest = coremap[i].pte->timestamp;
			evict = i;
		}
	}

	return evict;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	// set the age of a frame to current counter
	p->timestamp = counter;
	// increasing the counter
	counter++;
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	counter = 0;
	return;
}
