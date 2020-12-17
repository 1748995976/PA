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
  char str[1000];
  uint32_t result;
  while(!feof(fp)){
	  fscanf(fp,"%u",&result);
	  fscanf(fp,"%[^\n]",str);
	  bool success ;
	  printf("%s\n",str);
	  uint32_t count_result =  expr(str,&success);
	  printf("--------------%u %s %u\n",result,str,count_result);
	  //assert(count_result == result);
	  if(count_result == result)
      printf("yes\n");
	  else 
      printf("no\n");
	}
	fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);
  return 0;
}
