//
//  commands.c
//  MyShell
//
//  Created by James Johns on 10/12/2011.
//  Copyright 2011 xD-Duncesoft. All rights reserved.
//

#include "commands.h"
#include "builtin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/********************************************************************************
 * Function name  : int interpretCommand(char *buffer, FILE *source)            *
 *     returns    : zero on success, non-zero on failure                        *
 *         buffer     : An array of MAXCOMMANDLENGTH in size to store command   *
 *                      line contents in for later processing.                  *
 *         source     : File handle to read command line input from. If NULL is *
 *                      passed, stdin is used instead.                          *
 *                                                                              *
 * Created by     : James Johns                                                 *
 * Date created   : 10/12/2011                                                  *
 * Description    : Copies the contents of source into buffer line by line      *
 *                  (on subsequent calls, each line is read in order).          *
 *                                                                              *
 * NOTES          :                                                             *
 ********************************************************************************/
int readCommandLine(char *buffer, FILE *source) {
	printf("\n >");		/* command prompt character */
	if (buffer == NULL)
		buffer = malloc(sizeof(char)*MAXCOMMANDLENGTH);
	if (source == NULL)
		source = stdin;
	
	fgets(buffer, MAXCOMMANDLENGTH, source);
	if (buffer[0] != (char)NULL)	/* if we actually read something in */
		return 0;					/* return success */
	else
		return -1;
}


/********************************************************************************
 * Function name  : struct command_s *interpretCommand(char *commandLine)       *
 *     returns    : pointer to a command_s structure containing all command     *
 *                  data, ready to be passed to executeCommand()                *
 *         commandLine   : command line input to tokenise and return in command *
 *                         structure                                            *
 *                                                                              *
 * Created by     : James Johns                                                 *
 * Date created   : 10/12/2011                                                  *
 * Description    : tokenises commandLine by whitespace. first token is command *
 *                  utility, followed by arguments                              *
 *                                                                              *
 *                  returns tokens in command_s structure which needs           *
 *                  deallocating when no longer needed.                         *
 *                                                                              *
 * NOTES          : List TODOs, FIXES, BUGS, etc.                               *
 ********************************************************************************/
struct command_s *interpretCommand(char *commandLine) {
	
	struct command_s *toRet = malloc(sizeof(struct command_s));
	toRet->utility = NULL;
	toRet->argc = 0;
	toRet->argv = NULL;
	
	if (toRet) {
		int i;
		/* isolate command from the input */
		for (i = 0; i < strlen(commandLine) && commandLine[i] != ' ' &&
					commandLine[i] != '\n'; i++); /* count through characters until we reach the end 
												   * of the string or a tokenising character */
		toRet->utility = malloc(sizeof(char)*i);
		strncpy(toRet->utility, commandLine, i);
		
		/* if we hit a new line, then there is no more command to interpret. otherwise iterate past
		 * command utility and start parsing  */
		if (commandLine[i] == '\n')
			return toRet;
		else
			i++;
		char *tempStore[MAXARGUMENTCOUNT];
		unsigned int startOfToken = i;
		/* loop until we reach the end of the string or we hit a new line character
		 * tokenising the string at each space character */
		while ((commandLine[i] != '\n') && (i < strlen(commandLine))) {
			i++;
			if (commandLine[i] == ' ' || commandLine[i] == '\n') {
					// tokenise here
				unsigned int stringlength = (i-startOfToken);
				tempStore[toRet->argc] = malloc(sizeof(char)*(stringlength+1));
				strncpy(tempStore[toRet->argc], &commandLine[startOfToken], stringlength);
				tempStore[toRet->argc][stringlength] = '\0';
				toRet->argc++;
				startOfToken = i;
			}
		}
		
		toRet->argv = (char **) malloc(sizeof(char *)*(toRet->argc));
		for (int i = 0; i < toRet->argc; i++) {
			toRet->argv[i] = malloc(sizeof(char)*strlen(tempStore[i]));
			strncpy(toRet->argv[i], tempStore[i], strlen(tempStore[i]));
			free(tempStore[i]);
		}
	}
	return toRet;
}


/********************************************************************************
 * Function name  : int executeCommand(struct command_s *command)               *
 *     returns    : zero on success, non zero on error                          *
 *         command   : command_s structure as generated by interpretCommand()   *
 *                                                                              *
 * Created by     : James Johns                                                 *
 * Date created   : 10/12/2011                                                  *
 * Description    : Compare the utility described by command to several         *
 *                  builtins before attempting to run an  external executable   *
 *                  via fork/exec call.                                         *
 *                                                                              *
 * NOTES          : TODO - implement external executable calls via fork/exec    *
 *                  TODO - free command automagically since we allocated it     *
 *                         here in the first place. user may or may not         *
 *                         understand exactly what structure fields have been   *
 *                         dynamically allocated.                               *
 ********************************************************************************/
int executeCommand(struct command_s *command) {
	if (strcmp(command->utility, "pwd") == 0) {
		pwd();
	}
	else if (strcmp(command->utility, "cd") == 0) {
		cd("");
	}
	else if (strcmp(command->utility, "quit") == 0) {
		return -1;
	}
	else {
		printf("No fork/exec implemented yet. Cannot test environment for available utilities");
	}
	return 0;
}
