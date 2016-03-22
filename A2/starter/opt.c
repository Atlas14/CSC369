#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

#define MAXLINE 256

extern int debug;
extern struct frame *coremap;

static unsigned long counter; // keep tracking the timestamp
static unsigned long trace_length; 
static addr_t *frame_list; // store vaddr of each frame
static addr_t *trace_list; // store all trace vaddr 
static unsigned long trace_pos; //track postion the trace_list


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the i in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	
	int trace_left; // number of trace remaining
	
	int i,j;
	
	int victim = 0; 
	
	int max_dis = trace_length - trace_pos + 1;
	
	for(i = 0; i < memsize; i++) {
		
		coremap[i].pte->dis = max_dis;
		// update 'dis' for all frames
		trace_left = trace_length - trace_pos;
		for(j = 0; j < trace_left; j++ ){
			if (frame_list[i] == trace_list[trace_pos + j]){
				if ( (j < coremap[i].pte->dis ) ) {     
					coremap[i].pte->dis  = j;
					break;
				}
			}
		
		}
		// no occurence in remaining trace entry
		if(j == trace_length){
			victim = i;
			return victim;
		}
		
	}

	for(i = 0; i < memsize; i++){
		// use lru if the two have same dis
		if(coremap[i].pte->dis  == coremap[victim].pte->dis ){
			if(coremap[i].pte->timestamp > coremap[victim].pte->timestamp){
				victim = i;
			}
		}
		if(coremap[i].pte->dis  > coremap[victim].pte->dis ){
			victim = i;
		}
	}

	frame_list[victim] = 0;
				
	return victim;
						
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
 
void opt_ref(pgtbl_entry_t *p) {
	
	int frame; 
	
	frame = (p->frame >> PAGE_SHIFT); 
	// update frame info
	if (!frame_list[frame]) { 
		frame_list[frame] = trace_list[trace_pos];
	}
	
	p->timestamp = counter;
	// update counter and trace_pos
	counter++;	
	trace_pos += 1;

}



/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	
	int i = 0;

	// create a FILE and read tracefile
	FILE * tfp; 
	char buf[MAXLINE]; 
	if((tfp = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}
	
	//caluclate length of trace
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			trace_length++;
		}
	}
	
	// initial trace list which stores all intstruction from tracefile
	trace_list = malloc(trace_length * sizeof(addr_t)); 	
	
	fseek(tfp, 0, SEEK_SET); 
	
	i = 0;
	char type;
	addr_t vaddr; 
	// store vadder to trace_list
	while( fgets(buf, MAXLINE, tfp) != NULL) {  
		
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
	
			trace_list[i] = vaddr;
			i += 1;
		
		}
	}
	// intial frame_list which stores all frame
	frame_list = malloc(sizeof(addr_t) * memsize);
	while ( i < memsize ) {
	
		frame_list[i] = 0;
		i += 1;
	}	

	trace_pos = 0;
	
	fclose(tfp); 
}


