#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <errno.h>
#include "buddy.h"

int main(int argc, char *argv[]){
 //       for (int i = 0; i < 200; i++){
//		clock_t c_start, c_stop;
//		double time;
//		c_start = clock();
//
//		void *test = balloc(2000);
//		bfree(test);
//
//		c_stop = clock();
//		time = (double)(c_stop - c_start);
//		printf("%f\n", time);
//	}
	
	for (int i = 0; i < 200; i++){
		clock_t c_start, c_stop;
		double time;
		c_start = clock();

		void *test = malloc(2000);
		free(test);

		c_stop = clock();
		time = (double)(c_stop - c_start);
		printf("%f\n", time);
	}
}

