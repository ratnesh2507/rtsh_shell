#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define HISTORY_SIZE 100
#define RTSH_RL_BUFFER_SIZE 1024
#define RTSH_TOK_BUFSIZE 64
#define RTSH_TOK_DELIM " \t\r\n\a"

/*
  Function Declarations for builtin shell commands:
 */
int rtsh_cd(char **args);
int rtsh_help(char **args);
int rtsh_exit(char **args);
int rtsh_pwd(char **args);
int rtsh_clear(char **args);
int rtsh_history(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "pwd",
    "clear",
    "history"};

int (*builtin_func[])(char **) = {
    &rtsh_cd,
    &rtsh_help,
    &rtsh_exit,
    &rtsh_pwd,
    &rtsh_clear,
    &rtsh_history};

int rtsh_num_builtins()
{
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Command history support
 */
char *history[HISTORY_SIZE];
int history_count = 0;

void save_history(char *line)
{
  if (history_count < HISTORY_SIZE)
  {
    history[history_count++] = strdup(line);
  }
}

/*
  Builtin function implementations.
*/
int rtsh_cd(char **args)
{
  const char *path = args[1];
  char *home = getenv("HOME");

  if (path == NULL || strcmp(path, "~") == 0)
  {
    path = home;
  }
  else if (strcmp(path, "-") == 0)
  {
    path = getenv("OLDPWD");
    if (path)
      printf("%s\n", path);
  }

  if (path == NULL)
  {
    fprintf(stderr, "rtsh: cd: path not found\n");
    return 1;
  }

  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  if (chdir(path) != 0)
  {
    perror("rtsh");
  }
  else
  {
    setenv("OLDPWD", cwd, 1);
  }
  return 1;
}

int rtsh_help(char **args)
{
  int i;
  printf("BVK Ratnesh's rtsh - Custom Shell\n\n");
  printf("Built-in Commands:\n");
  printf("  cd [dir]   - Change the current directory\n");
  printf("  help       - Show this help menu\n");
  printf("  exit       - Exit the shell\n");
  printf("  pwd        - Print current working directory\n");
  printf("  clear      - Clear the terminal\n");
  printf("  history    - Show command history\n");
  return 1;
}

int rtsh_exit(char **args)
{
  return 0;
}

int rtsh_pwd(char **args)
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("%s\n", cwd);
  else
    perror("rtsh");
  return 1;
}

int rtsh_clear(char **args)
{
  printf("\033[H\033[J");
  return 1;
}

int rtsh_history(char **args)
{
  for (int i = 0; i < history_count; i++)
    printf("%d  %s\n", i + 1, history[i]);
  return 1;
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

int rtsh_execute(char **args)
{
  int i;

  if (args[0] == NULL)
  {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < rtsh_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return rtsh_launch(args);
}

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
        fprintf(stderr, "rtsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

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

void rtsh_prompt()
{
  char cwd[1024];
  getcwd(cwd, sizeof(cwd));
  char *user = getenv("USER");
  printf("%s@rtsh:%s$ ", user ? user : "user", cwd);
}

void rtsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do
  {
    rtsh_prompt();
    line = rtsh_read_line();
    save_history(line);
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
