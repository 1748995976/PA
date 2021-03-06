#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  if(args == NULL){
    cpu_exec(1);
  }else{
    int n;
    sscanf(args,"%d",&n);
    cpu_exec(n);
  }
  return 0;
}

static int cmd_info(char *args){
  void isa_reg_display();
  if(args == NULL){

  }else if(strcmp(args, "r") == 0){
    isa_reg_display();
  }else if(strcmp(args, "w") == 0){

  }
  return 0;
}

static int cmd_p(char* args){
	bool success = true;
	uint32_t result = expr(args, &success);
	if(success == true){
		printf("0x%x\n", result);
    return result;
	}
	else{
		printf("Invalid expression\n");
    return 0;
	}
}

static int cmd_x(char *args){
	char *args1 = strtok(NULL, " ");
	char *args2 = strtok(NULL, " ");
	if(args1 == NULL){
    panic("Lack first argument for command x");
  }else if(args2 == NULL){
    panic("Lack second argument for command x");
  }else{
		int cnt = atoi(args1);
		if(cnt<1){
      panic("Invalid first argument for command x");
    }else{
      bool success = true;
			uint32_t addr = expr(args2,&success);
      if(success == false){
        panic("Invalid address for command x");
      }else
      {
        for(int i=0; i<cnt; i++, addr+=4){
				  uint32_t data = isa_vaddr_read(addr,4);
				  printf("addr: %x -> %x\n",addr,data);
			  }
			  printf("\n");
      }
		}
	}
	return 0;
}

static int cmd_w(char *args){
	bool success = true;
	uint32_t result = expr(args, &success);
	if(success == true){
		WP *wp = new_wp();
		wp->old_val = result;
		strcat(wp->expression, args);
		printf("Watchpoint %d: %s -> %x\n", wp->NO, wp->expression, wp->old_val);
	}
	else{
		printf("Expression is not legal\n");
	}
	return 0;
}

static int cmd_d(char *args){
	int index_delete;
	sscanf(args, "%d", &index_delete);
	free_wp(index_delete);
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execution by step", cmd_si },
  { "info", "Print the value of all registers", cmd_info },
  { "p", "Print value of expression", cmd_p },
  { "x", "Scan Memory", cmd_x},
  { "w", "Add a watchpoint", cmd_w},
  { "d", "Delete a watchpoint", cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
