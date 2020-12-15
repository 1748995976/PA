#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_NEQ,
  TK_ADD,
  TK_SUB,
  TK_MUL,
  TK_DIV,
  TK_LP,
  TK_RP,
  TK_INT,
  TK_HEXINT,
  TK_REG,
  TK_AND,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
  int priority;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE, 0},
  {"==", TK_EQ, 2},
  {"!=", TK_NEQ, 2},
  {"\\+", TK_ADD, 3},
  {"\\-", TK_SUB, 3},
  {"\\*", TK_MUL, 4},
  {"\\/", TK_DIV, 4},
  {"\\(", TK_LP, 5},
  {"\\)", TK_RP, 5},
  {"[0-9]+", TK_INT, 0},
  {"0x[0-9|a-f|A-F]+", TK_HEXINT, 0},
  {"\\$[0-9|a-z|A-Z]+", TK_REG, 0},
  {"&&", TK_AND, 1}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  int priority;
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        assert(nr_token < 32);
        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_EQ:
          case TK_NEQ:
          case TK_ADD:
          case TK_SUB:
          case TK_MUL:
          case TK_DIV:
          case TK_LP:
          case TK_RP:
          case TK_AND:
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            nr_token++;
            break;
          case TK_INT:
          case TK_HEXINT:
          case TK_REG:
            assert(substr_len < 32);
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          default: 
            assert(substr_len < 32);
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            nr_token++;
            break;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p,int q){
  if(tokens[p].type != TK_LP || tokens[q].type != TK_RP){
    return false;
  }
  int cnt = 0;
  for (int i = p; i <= q; i++)
  {
    if(tokens[i].type == TK_LP){
      cnt++;
    }else if(tokens[i].type == TK_RP){
      cnt--;
    }
    if(cnt < 0)
      return false;
  }
  return cnt == 0;
}

int find_mainop(int p,int q){
  int ans = -1;//the position of main operator
  int priority = 6;
  for (int i = p; i <= q; i++)
  { 
    if(tokens[i].priority <= priority && tokens[i].priority != 0){
      ans = i;
      priority = tokens[i].priority;
    }
  }
  return ans;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
