# rtsh - BVK Ratnesh's Custom Shell

`rtsh` is a lightweight command-line shell written entirely in C in a single file. It serves as an educational project that mimics basic Unix shell behavior with a few built-in commands and support for external programs.

---

## ✨ Features

- Shell prompt in the format: `user@rtsh:/current/dir$`
- Built-in commands:

  - `cd [dir]` – Change directory (`~` and `-` supported)
  - `pwd` – Show current working directory
  - `clear` – Clear the screen
  - `help` – Show usage information
  - `history` – View up to 100 previous commands
  - `exit` – Exit the shell

- Executes external commands (e.g., `ls`, `cat`, `echo`, etc.)
- Graceful handling of memory and dynamic buffer allocation
- Modular design in a single file using static arrays and functions

---

## 🛠 Build and Run

### Requirements

- GCC or any C compiler
- Linux / Unix / macOS environment

### Compile

```bash
gcc rtsh.c -o rtsh
```

### Run

```bash
./rtsh
```

---

## 📚 Learning Objectives

- Understand the basic flow of a Unix shell: Read → Parse → Execute
- Practice with system calls like `fork()`, `execvp()`, `waitpid()`
- Manage strings and memory manually in C
- Implement reusable functions and maintain a minimal design

---

## 📦 Code Overview

| Component           | Description                           |
| ------------------- | ------------------------------------- |
| `rtsh_loop()`       | Main shell loop                       |
| `rtsh_prompt()`     | Prints the prompt                     |
| `rtsh_read_line()`  | Reads user input dynamically          |
| `rtsh_split_line()` | Tokenizes input                       |
| `rtsh_execute()`    | Executes built-in or external command |
| `rtsh_cd()`         | Changes current directory             |
| `rtsh_help()`       | Displays help text                    |
| `rtsh_history()`    | Shows in-memory history               |
| `rtsh_launch()`     | Handles process creation for commands |

---

## 🚀 Potential Future Features

- Persistent command history (e.g., saving to `.rtsh_history` file)
- Tab auto-completion
- Background process support using `&`
- Signal handling (e.g., `Ctrl+C`)
- I/O Redirection (`>`, `<`, `>>`) and piping (`|`)
- Aliases and scripting support

---

## 👨‍💻 Author

**BVK Ratnesh**
`rtsh` was built as a personal learning project in C to understand Unix shell design and system-level programming.

---

> "The best way to understand how something works is to build it from scratch."

---
