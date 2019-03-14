Gareth Peters
260678626
README.txt

OS used: linux.cs.mcgill.ca

1. Shell cannot process multiple inputs at once.
2. ./mysh < input.txt will prompt user for additional input if text file does not end with quit
3. Shell will not accept commands with too few or too many words.
4. My parse function is not in my shell.c file, but is instead in my interpreter.c file. This is also why I have no shell.h because there is no need. shell.c basically just acts as a main.c file.
