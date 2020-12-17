#include <stdio.h>
#include "nemu.h"
#include <stddef.h>
#include <errno.h>
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  FILE *fp = fopen("/home/ics2019/nemu/tools/gen-expr/input", "r");
	fclose(fp);

  return 0;
}
