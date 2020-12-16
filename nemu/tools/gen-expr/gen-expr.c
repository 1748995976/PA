#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

static inline uint32_t choose(uint32_t n){
	return rand()%n;
}

static char *gen_num()
{
    char *str = malloc(10);
    int num = choose(1000);
    sprintf(str, "%d", num);
    return str;
}

static char* gen_rand_op()
{
  int i = choose(4);
  char *operator = NULL;
  switch (i) {
    case 0:
      operator = strdup("+");
      break;
    case 1:
      operator = strdup("-");
      break;
    case 2:
      operator = strdup("*");
      break;
    case 3:
      operator = strdup("/");
      break;
  }
  return operator;
}


static char *gen_rand_expr()
{
    
  return str;
}
// this should be enough
static char buf[65536];

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result = fscanf(fp, "%d", &result);
    assert(result==1);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
