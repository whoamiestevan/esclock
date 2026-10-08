# esclock

A simple random key generator written in C for Linux.

## Features

- Generates a 200-character random key
- Uses `/dev/urandom`
- Supports letters, numbers, and symbols
- Prints the key to the terminal
- Saves the key to a file
- Prevents overwriting existing files

## Compile and Run

```bash
gcc esclock.c -o esclock
./esclock
