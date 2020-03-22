#include"sudoku_dancing_links.h"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer = PTHREAD_COND_INITIALIZER;
