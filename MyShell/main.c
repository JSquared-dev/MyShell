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
	char *commandLine = malloc(sizeof(char)*MAXCOMMANDLENGTH);
	for (int i = 0; i < MAXCOMMANDLENGTH; commandLine[i] = 0, i++);
	
	/* commandReturn is tested first as readCommandLine is blocking, therefore it would require a 
	 * new input before quiting instead of quiting on first request */
	while ((commandReturn != -1) && (readCommandLine(commandLine, stdin) != -1)) {
		struct command_s *command = interpretCommand(commandLine);
		if (command) {
			commandReturn = executeCommand(command);
			destroyCommand(command);
			command = NULL;
		}
		else {
			fprintf(stderr,"error interpretting command\n");
		}
	}
	free(commandLine);
    return 0;
}

