#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include<unistd.h>
#include<stdlib.h>

#include<pthread.h>
#include<sys/syscall.h> 
#include<sys/types.h>
#define THRDS 2 

#include "sudoku.h"

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void * func(void * args){
	FILE* fd=(FILE*)args;//打开文件后的指针
	
  char puzzle[128];
  int total_solved = 0;
  int total = 0;
  bool (*solve)(int) = solve_sudoku_basic;
	
  //读到文件
  //如果n大于一行的字符串长度，那么当读到字符串末尾的换行符时，
  //fgets(..)会返回。并且在s的最后插入字符串结束标志'\0'。 
  //而s缓冲区剩余的位置不会再填充。 
  while (fgets(puzzle, sizeof puzzle, fd) != NULL) {
    if (strlen(puzzle) >= N) {//N=81 
      ++total;
      input(puzzle);
      init_cache();
      //if (solve_sudoku_min_arity_cache(0)) {
      //if (solve_sudoku_min_arity(0))
      //if (solve_sudoku_basic(0)) {
      if (solve(0)) {
        ++total_solved;
        if (!solved())
          assert(0);
      }
      else {
        printf("No: %s", puzzle);
      }
    }
  }
	
}

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");//只读文件 
  
  char puzzle[128];
  int total_solved = 0;
  int total = 0;
  
  bool (*solve)(int) = solve_sudoku_basic;
  
  //第二个输入参数 应该输入d 
  if (argv[2] != NULL) 
//    if (argv[2][0] == 'a')
//      solve = solve_sudoku_min_arity;
//    else if (argv[2][0] == 'c')
//      solve = solve_sudoku_min_arity_cache;
    if (argv[2][0] == 'd')
      solve = solve_sudoku_dancing_links;

  int64_t start = now(); //计时开始 
  
  pthread_t ntid[THRDS];//线程id
  int err;
  for(long i=0;i<THRDS;i++){
  	err =  pthread_create(&ntid[THRDS], NULL, func, fp);
  	if(err!=0){
  		printf("ERROR: pthread_create() return %d\n", err);
        return -1;
	  }
  }
  for(long i=0; i<THRDS; i++){
        pthread_join(ntid[i], NULL);
  }
   
  
  
  int64_t end = now();  //计时结束 
  double sec = (end-start)/1000000.0;
  
  //输出时间 
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);

  return 0;
}

