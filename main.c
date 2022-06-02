#include <stdio.h>
#include <stdlib.h>

// If we are compiling on windows compile these function
#ifdef _WIN32
#include <string.h>

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

int main(int argc, char** argv) {

  // print version and exit info
  puts("Went Version 0.0.0.0.2");
  puts("Press Ctrl+c to Exit\n");

  while(1){
    // R.I.P golang
    char* input = readline("went> ");

    // Add input to history
    add_history(input);

    // Echo input back to user
    printf("%s\n", input);

    // Free retrieved input
    free(input);
  }

  return 0;

}
