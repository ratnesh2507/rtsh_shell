#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &rtsh_cd,
    &rtsh_help,
    &rtsh_exit};

int rtsh_num_builtins()
{
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int rtsh_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "rtsh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("rtsh");
    }
  }
  return 1;
}

int rtsh_help(char **args)
{
  int i;
  printf("BVK Ratnesh's rtsh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < rtsh_num_builtins(); i++)
  {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int rtsh_exit(char **args)
{
  return 0;
}

int rtsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0)
  {
    // Child process
    if (execvp(args[0], args) == -1)
    {
      perror("rtsh");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    // Error forking
    perror("rtsh");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

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

#define RTSH_TOK_BUFSIZE 64
#define RTSH_TOK_DELIM " \t\r\n\a"
char **rtsh_split_line(char *line)
{
  int bufsize = RTSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "rtsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, RTSH_TOK_DELIM);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize)
    {
      bufsize += RTSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "rtsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, RTSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
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