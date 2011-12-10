//
//  commands.h
//  MyShell
//
//  Created by James Johns on 10/12/2011.
//  Copyright 2011 xD-Duncesoft. All rights reserved.
//

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <stdio.h>

#define MAXCOMMANDLENGTH 1024

struct command_s {
	char *utility;
	unsigned int argc;
	char **argv;
};

int readCommandLine(char *command, FILE *source);
struct command_s *interpretCommand(char *commandLine);
int executeCommand(struct command_s *command);

#endif
