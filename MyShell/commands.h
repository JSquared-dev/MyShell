/****************************************************************************************
 * File name	: commands.h
 *
 * Created by	: James Johns
 * Date created	: 10/12/2011
 *
 * Description	: Command input, parsing and execution.
 *
 * NOTES		:	TODO - Tidy up command_s structure to allow piping, command 
 *							arguments and extensibility.
 ****************************************************************************************/

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdio.h>

#define MAXCOMMANDLENGTH 1024
#define MAXARGUMENTCOUNT 64

struct command_s {
	unsigned int argc; /* number of arguments in argv */
	char **argv; /* array of string arguments to pass to utility */
	struct command_s *next; /* next command in command list */
	int inputFD, outputFD; /* input and output file descriptors for stdio */
	int backgroundTask; /* if 1, send this command to background and recover keyboard access */
};

int readCommandLine(char **command, FILE *source);
struct command_s *interpretCommand(char *commandLine);
int executeCommand(struct command_s *command);
void destroyCommand(struct command_s *command);

#endif
