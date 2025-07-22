#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RTSH_RL_BUFFER_SIZE 1024
char *rtsh_read_line(void)
{
  int bufferSize = RTSH_RL_BUFFER_SIZE;
  int pos = 0;
  char *buffer = malloc(sizeof(char) * bufferSize);
  int c;

  if (!buffer)
  {
    fprintf(stderr, "rtsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    // Reading a character
    c = getchar();

    // If we hit EOF, replace it with a null char and return.
    if (c == EOF || c == '\n')
    {
      buffer[pos] = '\0';
      return buffer;
    }
    else
    {
      buffer[pos] = c;
    }
    pos++;
    // If we have exceeded the buffer, reallocate.
    if (pos >= bufferSize)
    {
      bufferSize += RTSH_RL_BUFFER_SIZE;
      buffer = realloc(buffer, bufferSize);
      if (!buffer)
      {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

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