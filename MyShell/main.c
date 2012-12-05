/****************************************************************************************
 * File name	: main.c
 *
 * Created by	: James Johns
 * Date created	: 10/12/2011
 *
 * Description	: Entry point for MyShell program. Runs a loop of reading command line 
 *					input, parsing and executing commands until quit command is given.
 *
 * NOTES		:	
 ****************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/param.h>
#include "builtin.h"
#include "commands.h"


int main (int argc, const char * argv[]) {
	
	int commandReturn = 0;
	char *commandLine;
	FILE *inputFile = NULL;
	int stdin_dup = -1, input_fd = -1;
	if (argc == 2) {
	  /* argv[1] is a script file */
	  inputFile = fopen(argv[1], "r");
	  if (inputFile != NULL) {
	  input_fd = fileno(inputFile);
	  stdin_dup = dup(STDIN_FILENO);
	  dup2(input_fd, STDIN_FILENO);
	  }
	  else {
	    perror("fopen");
	  }
	}
	/* commandReturn is tested first as readCommandLine is blocking, therefore it would require a 
	 * new input before quiting instead of quiting on first request */
	while ((commandReturn != -1) && (readCommandLine(&commandLine) != -1)) {
		struct command_s *command = interpretCommand(commandLine);
		if (command) {
			commandReturn = executeCommand(command);
			destroyCommand(command);
			command = NULL;
		}
		else {
			fprintf(stderr,"error interpretting command\n");
		}
	free(commandLine);
	}
	if (inputFile != NULL) {
	  dup2(stdin_dup, STDIN_FILENO);
	  fclose(inputFile);
	}
    return 0;
}

