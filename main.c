#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>


int main(int argc, char** argv) {

  // print version and exit info
  puts("Went Version 0.0.0.0.1");
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
