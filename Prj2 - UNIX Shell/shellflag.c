// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)


#define HISTORY_LENGTH 10
int TokCount = 0;
int HistCount = 0;
char MyHistoryies[HISTORY_LENGTH][COMMAND_LENGTH];

#define BUFFER_SIZE 50
static char buffer[BUFFER_SIZE];


/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	// if(length < 0 ){
	if(length < 0 && errno != EINTR){
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}


	//Singals interupt, dont update token
	if(length < 0 && errno == EINTR){
		return;
	}


	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}

	TokCount = token_count; //I added to count # of tokens
}



//writes history 
void Historywrite() {
	if(HistCount < HISTORY_LENGTH){ //if there isnt 10 itmes in history
		for(int i = 0; i < HistCount; i++){ 
			char chararr[COMMAND_LENGTH];
			sprintf(chararr, "%d\t%s\n", i+1, MyHistoryies[i]);
			write(STDOUT_FILENO, chararr, strlen(chararr));	
		}

	}else{ //more than 10 items in history
		for(int i = 0; i < 10; i++){
			int k = HistCount + i -9;
			char chararr[COMMAND_LENGTH];
			sprintf(chararr, "%d\t%s", k, MyHistoryies[i]);
			write(STDOUT_FILENO, chararr, strlen(chararr));
			write(STDOUT_FILENO, "\n", strlen("\n"));	
		}

	}
}


//adds commands into history arr
void HistoryAdd(char* commands[]) {
	if(commands[0][0] == '!'){ //dont add any "!n" or "!!" commands to history
		return;
	}

	char CommandArr[COMMAND_LENGTH]; //create a command array
	memset(&CommandArr[0], 0, sizeof(CommandArr));

	sprintf(CommandArr, "%s", commands[0]); //base case: add first segment of command

	for(int i = 1; i < TokCount; i++){ //adding following segments of command behind if any 
		//sprintf(CommandArr, "%s %s", CommandArr, commands[i]);
		strcat(CommandArr, " ");	
		strcat(CommandArr, commands[i]);
	}

	if (HistCount < HISTORY_LENGTH) {	//History length not at 10, add command line into history arr
		strcpy(MyHistoryies[HistCount], CommandArr);

	}else{
		for(int i = 0; i < HISTORY_LENGTH - 1; i++){ //history arr full, shift all up
			strcpy(MyHistoryies[i], MyHistoryies[i + 1]);
		}
		strcpy(MyHistoryies[HISTORY_LENGTH - 1], CommandArr);

	}

	HistCount++;
}


//run commands
void RunCommand(char* commands[]){

	int i = 0;

	if(strcmp(commands[0], "!!") == 0){
		if(HistCount < HISTORY_LENGTH){
			i = HistCount - 1; 
		}else{
			i = 9;
		}

	}else{
		if(strlen(commands[0]) == 1){ //catch error for just "!" input
			write(STDOUT_FILENO, "Error: Invalid history, no integer value \n", strlen("Error: Invalid history, no integer value \n"));
			return;
		}

		char* hold = strtok(commands[0], "!");
		int n = atoi(hold);
		if(n <= 0 || HistCount == 0 || n < HistCount - 10 || n > HistCount){ //catch error: if history is empty or user has an noninteger input or invalid integer input
			write(STDOUT_FILENO, "Error: Invalid history \n", strlen("Error: Invalid history \n"));
			return;
		}else{
			i = n - 1;
		}
	}

	char *previousTokens[NUM_TOKENS];
	int token_count = tokenize_command(MyHistoryies[i], previousTokens);

	for (int i = 0; i < token_count; i++) {
		write(STDOUT_FILENO, previousTokens[i], strlen(previousTokens[i]));
		write(STDOUT_FILENO, " ", strlen(" "));
	}

	write(STDOUT_FILENO, "\n", strlen("\n"));
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(previousTokens[token_count - 1], "&") == 0) {
		previousTokens[token_count - 1] = 0;
	}


	TokCount = token_count;

	HistoryAdd(previousTokens);


}



/* Signal handler function */ //from assignment
volatile sig_atomic_t flag = 0;
void handle_SIGINT()
{
	write(STDOUT_FILENO, "\n", strlen("\n"));
	Historywrite();
	write(STDOUT_FILENO, buffer, strlen(buffer));
	flag = 1; //send back flag and dont run internal commands
}



/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

	/* set up the signal handler */ //from assignment
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);

	// printf("Program now waiting for Ctrl-C.\n");
	// while (1)
	// ;


	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().


		//Added to get the get directory line in front 
		char pwdcom[COMMAND_LENGTH];
		if (getcwd(pwdcom, sizeof(pwdcom)) != NULL) {
			write(STDOUT_FILENO, pwdcom, strlen(pwdcom));
			write(STDOUT_FILENO, "$ ", strlen("$ "));
		}else{
			write(STDOUT_FILENO, "Error: getcwd() returned NULL. \n", strlen("Error: getcwd() returned NULL. \n"));
		}

		_Bool in_background = false;

		read_command(input_buffer, tokens, &in_background);

		// if (skip) {
		// 	skip = false;
		// 	continue;
		// }

		// // DEBUG: Dump out arguments:
		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	// write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	// write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	// write(STDOUT_FILENO, "\n", strlen("\n"));
		// 	TokCount++;
		// }
		// if (in_background) {
		// 	write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		// }

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */ 
		
		

		//Test: Write TokCount
		// char chararr[COMMAND_LENGTH];
		// sprintf(chararr, "%d", TokCount);
		// write(STDOUT_FILENO, chararr, strlen(chararr));
		


		//4. Internal Commands
		if(tokens[0] != NULL && flag != 1){	//catch empty line seg fault, do nothing, catches signal and skips internal commands

			HistoryAdd(tokens);

			if(strcmp(tokens[0], "exit") == 0){ 	// exit shell
				exit(0);

			}else if(strcmp(tokens[0], "pwd") == 0){ 	// pwd, prints the current directory, using getcwd()
				if(getcwd(pwdcom, sizeof(pwdcom)) != NULL) {
					write(STDOUT_FILENO, pwdcom, strlen(pwdcom));
					write(STDOUT_FILENO, "\n", strlen("\n"));
				}else{
					write(STDOUT_FILENO, "Error: getcwd() returned NULL. \n", strlen("Error: getcwd() returned NULL. \n"));
				}

			}else if(strcmp(tokens[0], "cd") == 0){ // cd, changes directory using chdir()
				if (chdir(tokens[1]) != 0) { //if entered invalid directory
					write(STDOUT_FILENO, "Invalid directory. \n", strlen("Invalid directory. \n"));
				}

			}else if(strcmp(tokens[0], "type") == 0 ){ // type, output the type, buitin or external
				if(tokens[1] != NULL) { // segfault catch if user only enter "type"
					if(strcmp(tokens[1], "exit") == 0 || strcmp(tokens[1], "pwd") == 0  || strcmp(tokens[1], "cd") == 0  || strcmp(tokens[1], "history") == 0 ){
						write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
						write(STDOUT_FILENO, " is a shell300 builtin \n", strlen(" is a shell300 builtin \n"));
					}else{
						write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
						write(STDOUT_FILENO, " is external to shell300 \n", strlen(" is external to shell300 \n"));
					}

				}else{
					write(STDOUT_FILENO, "Error: Not a valid type. \n", strlen("Error: Not a valid type. \n"));
				}

			}else if(strcmp(tokens[0], "history") == 0){// history, do history stuff
				Historywrite();

			}else if(tokens[0][0] == '!'){ //run !n && !! commands
		
				RunCommand(tokens);

			}else{ // run child for shell 300 external commands (ls, cat, clear)


				//3. Creating child process, forking from our notes
				pid_t mypid;
				mypid = fork(); //fork another process

				if(mypid < 0){ //error: fork failed
					write(STDOUT_FILENO, "Error: execvp() returns an error, fork failed. \n", strlen("Error: execvp() returns an error, fork failed. \n")); //error message
					exit(-1);
				}else if(mypid == 0){ //child process
					execvp(tokens[0], tokens);
					write(STDOUT_FILENO, tokens[0], strlen(tokens[0])); // output for unknown command
					write(STDOUT_FILENO, ": Unknown command. \n", strlen(": Unknown command. \n")); 
					exit(0);
				}else{ //parent
					if (!in_background) { //waiting for child to complete
    					waitpid(mypid, NULL, 0); 
					}
					// Taken from assignment // Cleanup any previously exited background child processes // (The zombies)
					while (waitpid(-1, NULL, WNOHANG) > 0) 
						; // do nothing.
				}


			}	


		}		

	flag = 0;
	}
	return 0;
}