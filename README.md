# WebServ – Mini HTTP Server (C++98)

> **⚠️ Disclaimer**
> This project is a small learning‑project written in **C++98** and intentionally avoids modern language features to keep the build chain minimal. It is not intended for production use.

---

## Table of Contents

1. [What Is It?](#what-is-it)
2. [Prerequisites](#prerequisites)
3. [Project Layout](#project-layout)
4. [Building the Project](#building-the-project)
5. [Running the Server](#running-the-server)
6. [Debugging with Valgrind](#debugging-with-valgrind)
7. [Cleaning Up](#cleaning-up)
8. [Extending the Codebase](#extending-the-codebase)
9. [FAQ & Troubleshooting](#faq--troubleshooting)

---

## What Is It?

`webserv` is a lightweight HTTP/1.1‑compatible web server written in **C++98**.
It demonstrates:

- Basic file parsing (configuration files)
- Signal handling (`SIGINT` → graceful shutdown)
- Logging with different verbosity levels
- A modular architecture: _Parser_, _Server_, _Logger_, etc.

Feel free to use it as a reference or as the starting point for your own projects!

---

## Prerequisites

| Component               | Minimum Version                                    | Why?                                          |
| ----------------------- | -------------------------------------------------- | --------------------------------------------- |
| C++ compiler            | `g++` **≥ 4.8** (or any compiler supporting C++98) | Compiles the source code.                     |
| `make`                  | Any modern `make`                                  | Build system defined in `Makefile`.           |
| `valgrind` _(optional)_ | Latest stable release                              | Run with `make start` for memory‑leak checks. |

> **Tip:** On Debian/Ubuntu you can install them with:
>
> ```bash
> sudo apt update && sudo apt install build-essential valgrind
> ```

---

## Project Layout

```
webserv/
├── Makefile
├── README.md
├── includes/
│   ├── Logger/
│   ├── Parsing/
│   └── Usefull/
├── srcs/
│   ├── Arguments.cpp
│   ├── ConfParser.cpp
│   ├── Log.cpp
│   ├── Server.cpp
│   └── main.c
└── obj/          ← created by the build process
```

- **`srcs/`** – All `.cpp/.c` source files.
- **`includes/`** – Header files for each module.
- **`obj/`** – Object files generated during compilation (auto‑created).

The `Makefile` automatically scans `srcs/` for all `.cpp` files, so you don’t need to edit it when adding new sources.

---

## Building the Project

```bash
# Compile and link everything into a single binary called 'webserv'
make          # or: make all
```

You’ll see coloured progress messages in the terminal (if your shell supports ANSI colours).
If something fails, inspect the error message; often it’s a missing header or a typo.

---

## Running the Server

The server expects at least one command‑line argument:

1. **Configuration file path** – `config/goodweb.conf` is an example in this repository.
2. Optional flag: `--debug` → prints parsed configuration to stdout.

```bash
# Basic usage
./webserv config/goodweb.conf

# With debug output
./webserv config/goodweb.conf --debug
```

### Command‑Line Options Summary

| Option          | Description                                           |
| --------------- | ----------------------------------------------------- |
| `<config_file>` | Path to the server configuration file. Required.      |
| `--debug`       | Dump parsed configuration and enable verbose logging. |

> **NOTE**
> The binary is created in the project root (`./webserv`). If you’re running from a different directory, adjust the path accordingly.

---

## Debugging with Valgrind

The Makefile exposes a convenience target:

```bash
make start
```

This runs `valgrind -q ./webserv config/goodweb.conf --debug` automatically.
If you prefer manual control, use:

```bash
valgrind -q ./webserv config/goodweb.conf --debug
```

Valgrind will suppress output (`-q`) but will still report any memory leaks or invalid accesses at the end of execution.

---

## Cleaning Up

| Target        | What It Does                                      |
| ------------- | ------------------------------------------------- |
| `make clean`  | Deletes only the `obj/` directory (object files). |
| `make fclean` | Deletes `obj/` **and** the binary (`webserv`).    |

```bash
# Remove object files only
make clean

# Full cleanup, ready for a fresh build
make fclean
```

---

## Extending the Codebase

1. **Add a new source file**

   ```bash
   # Example: create a new module in srcs/
   touch srcs/NewModule.cpp
   echo '#include "NewModule.hpp"' > srcs/NewModule.cpp
   ```

2. **Create its header** (`includes/` or appropriate sub‑folder).

3. **Compile** – no need to modify `Makefile`; the `find` command pulls it in automatically.

4. **Link against other modules** by adding `#include` directives and using them in your code.

---

## FAQ & Troubleshooting

| Question                                                                        | Answer                                                                                                                                                |
| ------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Why does my build fail on a recent GCC?**                                     | The project uses C++98 flags (`-std=c++98`). Newer compilers might still support it, but ensure you’re not inadvertently using `-std=c++11` or later. |
| **The server doesn’t start; I get “No such file or directory” for the config.** | Verify that the path is correct relative to where you run the binary. Use absolute paths if needed.                                                   |
| **How do I change the logging level?**                                          | Look into `Log.hpp`/`Log.cpp`. The code uses a static flag (`Log::getLogDebugState()`). Pass `--debug` or modify the source to toggle it.             |
| **Signal handling doesn’t work on Windows.**                                    | The current code uses POSIX signals (`signal(SIGINT, ...)`). On Windows you’ll need an alternative (e.g., `SetConsoleCtrlHandler`).                   |
| **Valgrind reports a leak even though I think I cleaned everything.**           | Run with `--leak-check=full` to see detailed information. The server might keep sockets open; ensure `close()` is called on shutdown.                 |

---

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

---
