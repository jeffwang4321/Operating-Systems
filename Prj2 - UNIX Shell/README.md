# Shell with History
In this project, you will develop a simple UNIX shell. The shell accepts user commands and then executes each
command in a separate process. The shell provides the user a prompt at which the next command is entered. One
technique for implementing a shell interface is to have the parent process first read what the user enters on the
command line and then create a separate child process that performs the command. Unless otherwise specified,
the parent process waits for the child to exit before continuing.

Internal commands are commands that the shell itself implement, as opposed to a separate program that is
executed. Implement the commands listed below. Note that for these you need not fork a new process as they
can be handled directly in the parent process.

The next task is to modify your shell to provide a history feature that allows the user access up to the ten most
recently entered commands. Start numbering the user's commands at 1 and increment for each command
entered. Next, implement the ! commands which allows users to run commands directly from the history list

Change your shell program to display the list of history commands when the user presses ctrl-c (which is the
SIGINT signal)
