#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <queue>

#include "sudoku_dancing_links.h"
#include "sudoku.h"

int TH=10;//线程数量

char puzzle[128];

int64_t now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void *solve(void *args) {
    pthread_mutex_lock(&mut);
    
    //pthread_cond_wait(&consumer,&mut);
    
    Dance *d = (Dance *) args;
    d->solve();
    for (int i = 0; i < 81; i++) {
        printf("%d", d->inout_[i]);
    }
    printf("%c", '\n');
    //pthread_cond_signal(&consumer);
    pthread_mutex_unlock(&mut);
}


int main(int argc, char *argv[]) {
    init_neighbors();

    char *str = (char *) malloc(20 * sizeof(char));
    scanf("%s", str);
    printf("opening %s\n", str);

    FILE *fp = fopen(str, "r");

    if(fp == nullptr) {
        printf("null\n");
        exit(-1);

    }
    int total = 0;

    queue <pthread_t> threadQue;
    int64_t start = now();
    
    while (fgets(puzzle, sizeof puzzle, fp) != NULL) {
        if (strlen(puzzle) >= N) {
            ++total;
            input(puzzle);
            init_cache();
	    
            Dance *d = new Dance(puzzle);

	    if(threadQue.size()>=TH)
	    {	
		pthread_cond_wait(&consumer,&mut);
	    }
	    else
	    {
	        pthread_t th;
	        pthread_create(&th, NULL, solve, d);
                threadQue.push(th);
	    }

	    if(threadQue.size()>=TH)
	    {
		while(!threadQue.empty())
		{
		    pthread_t th=threadQue.front();
		    pthread_join(th, NULL);
		    threadQue.pop();
		    pthread_cond_signal(&consumer);
		}
	    }
        }
    }
 
	while(!threadQue.empty())
	{
	    pthread_t th=threadQue.front();
	    pthread_join(th, NULL);
	    threadQue.pop();
	    pthread_cond_signal(&consumer);
	}

    int64_t end = now();
    double sec = (end - start) / 1000000.0;
    printf("%f sec %f ms each %d\n", sec, 1000 * sec / total, total);

    free(str);

    return 0;
}

