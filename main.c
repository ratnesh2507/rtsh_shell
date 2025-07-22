#include <stdlib.h>

void rtsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do
  {
    printf("> ");
    line = rtsh_read_line();
    args = rtsh_split_line(line);
    status = rtsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  rtsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}