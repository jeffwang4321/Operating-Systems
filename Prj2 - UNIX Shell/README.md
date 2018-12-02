# UNIX Shell with Command History
A simple UNIX shell that accepts user commands and then executes each command in a separate process. The shell provides the user a prompt at which the next command is entered. The shell interface is implemented by having the parent process first read what the user enters on the command line and then creating a separate child process that performs the command. The parent process then waits for the child to exit before continuing.

Four internal commands are implemented by this shell, and are this UNIX shell's builtin commands. These commands do not need to fork a new process as they can be handled directly in the parent process.
- exit - Exit the program. Does not matter how many arguments the user enters
- pwd - Display the current working directory using the getcwd() function
- cd - Changes the current working directory to the desired directory using the chdir() function. Passes chdir() the first argument the user enters (it will accept absolute and relative paths)
- type - Display information about command type

Provides a history feature that allows the user access up to the ten most recently entered commands
- history - Displays the list of history commands, up to the ten most recently entered commands
- !n - Allows users to run commands directly from the history list (where "n" is the n-th number command)
- !! - Runs the last most recent command (previous)

Investigated signals and implemented signal handlers. Shell program to display the list of history commands when the user presses ctrl-c (which is the SIGINT signal)
