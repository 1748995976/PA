#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

extern uint32_t isa_reg_str2val(const char *s, bool *success);
uint32_t eval(int p, int q, bool *success);
uint32_t getOp(int p, int q);
uint32_t getPrior(int op);
bool checkOp(int k);
bool check_parentheses(int i, int j);
bool isDeref(int k);
enum {
    TK_NOTYPE = 256, TK_EQ,
    /* TODO: Add more token types */
            TK_NEQ, TK_AND, TK_REG, TK_HEXNUM, TK_NUM, TK_NEG,
    TK_DEREF
};
static struct rule {
    char *regex;
    int token_type;
} rules[] = {

        /* TODO: Add more rules.
         * Pay attention to the precedence level of different rules.
         */
        {"\\$[\\$0-9a-zA-z]+", TK_REG}, //registers: Can be more specific
        {"0[xX][0-9a-fA-F]+",  TK_HEXNUM},  //Hex number: Must before number
        {"[0-9]+",             TK_NUM}, //numbers
        {" +",                 TK_NOTYPE},    // spaces
        {"\\+",                '+'},         // plus
        {"==",                 TK_EQ},         // equal
        {"-",                  '-'},         //
        {"\\*",                '*'},
        {"/",                  '/'},
        {"!=",                 TK_NEQ},
        {"&&",                 TK_AND},
        {"\\(",                '('},
        {"\\)",                ')'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )
#define MAX_VALUE 128

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
    int i;
    char error_msg[128];
    int ret;

    for (i = 0; i < NR_REGEX; i++) {
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
} Token;

static Token tokens[2014] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
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

                switch (rules[i].token_type) {
                    case TK_NUM:
                    case TK_HEXNUM:
                    case TK_REG:
                        if (substr_len >= 32) {
                            printf("illegal number!!!\n");
                            assert(0);
                        }
                        memset(tokens[nr_token].str, '\0', 32);
                        strncpy(tokens[nr_token].str, substr_start, substr_len);;
                    default:
                        tokens[nr_token].type = rules[i].token_type;
                        nr_token++;
                    case TK_NOTYPE:
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

uint32_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    /* TODO: Insert codes to evaluate the expression. */
    for (int i = 0; i < nr_token; i++) {
        if (tokens[i].type == '*' && (i == 0 || isDeref(i - 1))) {
            tokens[i].type = TK_DEREF; //指针
        }
        if (tokens[i].type == '-' && (i == 0 || isDeref(i - 1))) {
            tokens[i].type = TK_NEG; //负数
        }
    }
    return eval(0, nr_token - 1, success);
}

uint32_t eval(int p, int q, bool *success) {
    if (p > q) {
        /* Bad expression */
//        Log("p: %d, q: %d", p, q);
        *success = false;
        return 0;
    } else if (p == q) {
        /* Single token.
         * For now this token should be a number.
         * Return the value of the number.
         */
        uint32_t res;
        if (tokens[p].type == TK_REG) {//reg
            bool tmp = false;
            res = isa_reg_str2val(&(tokens[p].str[1]), &tmp);
            if(!tmp){
                *success = false;
                return 0;
            }
        } else if (tokens[p].type == TK_HEXNUM) {//0xffff
            sscanf(&(tokens[p].str[2]), "%x", &res);
        } else {//num
            sscanf(tokens[p].str, "%d", &res);
        }
        return res;
    } else if (check_parentheses(p, q) == true) {
        /* The expression is surrounded by a matched pair of parentheses.
         * If that is the case, just throw away the parentheses.
         */
        return eval(p + 1, q - 1, success);
    } else {
        uint32_t op;
        uint32_t val1, val2;
        op = getOp(p, q);
        if (tokens[op].type == TK_DEREF) {
            uint32_t val = eval(op + 1, q, success);
           if(val < 0x80100000){
               assert(0);
           }
          return paddr_read(val, 4);
        }
        if (tokens[op].type == TK_NEG) {
            int val = eval(p + 1, q, success);
            return -val;
        }
        val1 = eval(p, op - 1, success);
        val2 = eval(op + 1, q, success);

        switch (tokens[op].type) {
            case '+':
                return val1 + val2;
            case '-':
                return val1 - val2;
            case '*':
                return val1 * val2;
            case '/':
                if (val2 == 0) {
                    assert(0);
                }
                return val1 / val2;
            case TK_EQ :
                return val1 == val2;
            case TK_NEQ :
                return val1 != val2;
            case TK_AND :
                return val1 && val2;
            default:
                *success = false;
        }
    }
}

uint32_t getOp(int p, int q) {
    int count = 0;
    int curpos = p;
    int prior = MAX_VALUE;
    for (int k = p; k <= q; k++) {
        if (tokens[k].type == '(') {
            count++;
            continue;
        }
        if (tokens[k].type == ')') {
            count--;
            continue;
        }
        Log("count == %d", count);
        if (count == 0 && checkOp(k)) {
            Log("TYPE == %d", tokens[k].type);
            int tmp = getPrior(tokens[k].type);
            Log("prior == %d", tmp);
            if (tmp <= prior) {
                Log("curpos == %d", k);
                curpos = k;
                prior = tmp;
            }
        }
    }
    Log("final pos: %d", curpos);
    return curpos;
}

bool checkOp(int k) {
    switch (tokens[k].type) {
        case '+':
        case '-':
        case '*':
        case '/':
        case TK_EQ:
        case TK_NEQ:
        case TK_AND:
        case TK_DEREF:
            Log("k= %d checkOp---> true", k);
            return true;
        default:
            Log("k= %d checkOp---> false", k);
            return false;
    }
}


uint32_t getPrior(int op) {
    switch (op) {
        case TK_NEG:
            return 6;
        case TK_DEREF:
            return 5;
        case TK_AND :
            return 4;
        case '*':
        case '/':
            return 3;
        case TK_EQ:
        case TK_NEQ:
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return MAX_VALUE;
    }
}

bool check_parentheses(int i, int j) {
    if (tokens[i].type != '(' || tokens[j].type != ')')
        return false;
    int k = i + 1, count = 0;
    for (; k < j; k++) {
        if (tokens[k].type == '(') {
            count++;
        } else if (tokens[k].type == ')') {
            count--;
        }
        if (count < 0) {
            return false;
        }
    }
    if (count != 0) {
        Log("check_parentheses---> false");
        return false;
    }
    Log("check_parentheses---> true");
    return true;
}

bool isDeref(int k) {
    switch (tokens[k].type) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '(':
        case TK_EQ:
        case TK_NEQ:
        case TK_DEREF:
        case TK_NEG:
            return true;
        default:
            return false;;
    }
}