#include <stdio.h>
#include "nemu.h"
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  FILE *fp = fopen("/home/hust/ics2019/nemu/tools/gen-expr/input","r");
  while(!feof(fp)){
    char str[1000];
    uint32_t result;
    bool success ;
	  fscanf(fp,"%u",&result);//first read right result
	  fscanf(fp,"%[^\n]",str);//second read expression
	  uint32_t count_result =  expr(str,&success);
	  printf("%u %s %u\n",result,str,count_result);
	  if(count_result == result)
      printf("success!\n");
	  else 
      printf("failed!\n");
	}
	fclose(fp);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);
  return 0;
}
