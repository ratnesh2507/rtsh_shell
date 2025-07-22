#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *rtsh_read_line(void)
{
#ifdef RTSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1)
  {
    if (feof(stdin))
    {
      exit(EXIT_SUCCESS); // We received an EOF
    }
    else
    {
      perror("rtsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define RTSH_RL_BUFFER_SIZE 1024
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
    // Read a character
    c = getchar();

    if (c == EOF)
    {
      exit(EXIT_SUCCESS);
    }
    else if (c == '\n')
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
#endif
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