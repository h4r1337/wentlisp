#include "mpc.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>

// If we are compiling on windows compile these function
#ifdef _WIN32

static char buffer[2048]

// Fake readline function
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

// Fake add_history fnctinon
void add_history(char unused) {}

// Otherwise include the editline headers
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

typedef struct {
  int type;
  long num;
  int err;
} lval;

// enum for type
enum { LVAL_NUM, LVAL_ERR };
// enum for err
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// create a new num type lval
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

// create a new er type lval
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

// Print lval
void lval_print(lval v) {
  switch (v.type) {
    case LVAL_NUM: printf("%li", v.num); break;
    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if (v.err == LERR_BAD_OP) {
        printf("Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM) {
        printf("Error: Invalid Number!");
      }
      break;
  }
}


void lval_println(lval v) { lval_print(v); putchar('\n'); }


lval eval_op(lval x, char* op, lval y) {
  // if either value is an error return it
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }
  // othewise do math on the number values
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
  if (strcmp(op, "/") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0) { 
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num % y.num);
  }

  return lval_err(LERR_BAD_OP);
}


lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    // Check if there is some error in conversion
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}


int main(int argc, char** argv) {

  // Creating some parsers
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  // Polish notation grammer
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number   : /-?[0-9]+/ ;                             \
      operator : '+' | '-' | '*' | '/' | '%' | '^';       \
      expr     : <number> | '(' <operator> <expr>+ ')' ;  \
      lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
    Number, Operator, Expr, Lispy);

  // print version and exit info
  puts("Went Version 0.0.0.0.3");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    /* Attempt to parse the user input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);

    } else {
      /* Otherwise print and delete the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
  }
  
  // Undefine and delete our parsers
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;

}