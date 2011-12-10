//
//  main.c
//  MyShell
//
//  Created by James Johns on 09/12/2011.
//  Copyright 2011 xD-Duncesoft. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/param.h>
#include "builtin.h"
#include "commands.h"


int main (int argc, const char * argv[])
{
	
		// insert code here...
	int commandReturn = 0;
	char *commandLine = malloc(sizeof(char)*MAXCOMMANDLENGTH);
	for (int i = 0; i < MAXCOMMANDLENGTH; commandLine[i] = 0, i++);
	
	/* commandReturn is tested first as readCommandLine is blocking, therefore it would require a 
	 * new input before quiting instead of quiting on first request */
	while ((commandReturn != -1) && (readCommandLine(commandLine, stdin) != -1)) {
		struct command_s *command = interpretCommand(commandLine);
		if (command) {
			commandReturn = executeCommand(command);
			free(command);
		}
	}
	free(commandLine);
    return 0;
}

