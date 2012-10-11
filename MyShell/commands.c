/****************************************************************************************
 * File name	: commands.c
 *
 * Created by	: James Johns
 * Date created	: 10/12/2011
 *
 * Description	: Command input, parsing and execution.
 *
 * NOTES		:	TODO - Create pipes to allow output from one command to be the input 
 *							of another when using the pipe delimiter (|).
 *					TODO - Tidy up memory allocations to allocate necessary amount of 
 *							data instead of limited lengths.
 ****************************************************************************************/

#include "commands.h"
#include "builtin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/********************************************************************************
 * Function name  : int interpretCommand(char *buffer, FILE *source)
 *     returns    : zero on success, non-zero on failure
 *         buffer     : A char array of MAXCOMMANDLENGTH in size to store
 *                      command line contents in for later processing.
 *         source     : File handle to read command line input from. If NULL is
 *                      passed, stdin is used instead.
 * 
 * Created by     : James Johns
 * Date created   : 10/12/2011
 * Description    : Copies the contents of source into buffer line by line
 *                  (on subsequent calls, each line is read in order).
 *
 * NOTES          : 
 ********************************************************************************/
int readCommandLine(char *buffer, FILE *source) {
	printf("\n >");		/* command prompt character */
	if (source == NULL)
		source = stdin;
	
	fgets(buffer, MAXCOMMANDLENGTH, source);
	if (buffer[0] != (char)NULL)	/* if we actually read something in */
		return 0;					/* return success */
	else
		return -1;
}


/********************************************************************************
 * Function name  : struct command_s *interpretCommand(char *commandLine)
 *     returns    : pointer to a command_s structure containing all command
 *                  data, ready to be passed to executeCommand()
 *         commandLine   : command line input to tokenise and return in command
 *                         structure
 *
 * Created by     : James Johns
 * Date created   : 10/12/2011
 * Description    : tokenises commandLine by whitespace. first token is command
 *                  utility, followed by arguments
 *
 *                  returns tokens in command_s structure which needs
 *                  deallocating when no longer needed.
 *
 * NOTES          : 
 ********************************************************************************/
struct command_s *interpretCommand(char *commandLine) {
	
	struct command_s *toRet = malloc(sizeof(struct command_s));
	/* temporary argument storage. ready to copy into a perfect sized array later on */
	char *tempStore[MAXARGUMENTCOUNT];
	unsigned int startOfToken = 0;
	
	if (toRet == NULL) {
		perror("interpretCommand: malloc");
	}
	else {
		toRet->next = NULL;
		toRet->argc = 0;
		toRet->argv = malloc(sizeof(char *)*2);
		if (toRet->argv == NULL) {
			perror("interpretCommand: malloc");
			free(toRet);
			return NULL;
		}
		
		int i = 0;
		/* ignore leading whitespace */
		while ((i < strlen(commandLine)) && ((commandLine[i] == ' ') || (commandLine[i] == '\n')))
			i++;
		
		if (i >= strlen(commandLine)) {
			/* no command to parse, so ignore it and return NULL */
			fprintf(stderr, "Empty command detected\n");
			free(toRet);
			toRet = NULL;
			return NULL;
		}
		startOfToken = i;
		
		/* could characters in command name, ready to allocate required space. */
		for (; i < strlen(commandLine) && commandLine[i] != ' ' && 
		       commandLine[i] != '\n'; i++); /* count through characters until we reach the end 
						      * of the string or a tokenising character */
		
		toRet->argv[0] = malloc(sizeof(char)*(i+1));
		toRet->argc++;
		strncpy(toRet->argv[0], &commandLine[startOfToken], i-(startOfToken)); /* startOfToken is an index. add one for calculating length properly */
		toRet->argv[0][i] = NULL;
		toRet->argv[1] = NULL; /* make sure even for single commands, argv[] ends in a NULL pointer */
		
		i++;
		startOfToken = i;
		
		/* loop until we reach the end of the string or we hit a new line character
		 * tokenising the string at each space character */
		while (i < strlen(commandLine)) {
			switch (commandLine[i]) {
				case ' ':
				case '\n':
				case '\0':
					/* if 2 separators are found next to eachother, ignore the first separator and 
					 * use the second as the startOfToken point. */
					if (i == startOfToken) {
						i++;
						startOfToken = i;
						break;
					}
					/* create a token from string */
					tempStore[toRet->argc] = malloc(sizeof(char)*(i-startOfToken)+1);
					strncpy(tempStore[toRet->argc],commandLine+startOfToken,i-startOfToken);
					tempStore[i-startOfToken] = '\0';
					toRet->argc++;
					/* set records to beginning of the next possible token */
					startOfToken = i;
					break;
				case '|':
					toRet->next = interpretCommand(commandLine+i+1);
					i = strlen(commandLine);
					break;
					
				default:
					i++;
					break;
			}
		}
		
		/* copy arguments into command structure */
		if (toRet->argc > 1) {
			toRet->argv = realloc(toRet->argv, sizeof(char *)*(toRet->argc+1));
			/* loop through tempStore, copying all strings across to argv */
			for (int j = 1; j < toRet->argc; j++) {
				toRet->argv[j] = tempStore[j];
			}
			toRet->argv[toRet->argc] = NULL;
		}
	}
	return toRet;
}


/********************************************************************************
 * Function name  : int executeCommand(struct command_s *command)
 *     returns    : zero on success, non zero on error
 *         command   : command_s structure as generated by interpretCommand()
 *
 * Created by     : James Johns
 * Date created   : 10/12/2011
 * Description    : Compare the utility described by command to several
 *                  builtins before attempting to run an  external executable
 *                  via fork/exec call.
 *
 * NOTES          : TODO - implement external executable calls via fork/exec
 *                  TODO - free command automagically since we allocated it
 *                         here in the first place. user may or may not
 *                         understand exactly what structure fields have been 
 *                         dynamically allocated.
 ********************************************************************************/
int executeCommand(struct command_s *command) {
	int inputFD, outputFD;
	inputFD = fileno(stdin);
	outputFD = fileno(stdout);
	if (strcmp(command->argv[0], "pwd") == 0) {
		pwd(command->argc, command->argv, inputFD, outputFD);
	}
	else if (strcmp(command->argv[0], "cd") == 0) {
		cd(command->argc, command->argv, inputFD, outputFD);
	}
	else if ((strcmp(command->argv[0], "quit") == 0) || (strcmp(command->argv[0], "exit") == 0)) {
		return -1;
	}
	else {
		int previousOutputPipe = inputFD;
		while (command != NULL) {
			printf("Executing fork/exec for command: %s\n", command->argv[0]);
			int datapipe[2];
			if (pipe(datapipe) == -1) {
				perror("pipe");
				return 1;
			}
			
			/* if end of command string, print final output. otherwise continue redirection */
			if (command->next == NULL) {
				/* close datapipe[0] and use stdout for process output */
				close (datapipe[0]);
				datapipe[0] = outputFD;
			}
			
			executeExternalCommand(command->argc, command->argv, previousOutputPipe, datapipe[1]);
			
			/* save read end of current pipe for use as an input in the next command. */
			if (previousOutputPipe != inputFD) {
				close(previousOutputPipe);
			}
			previousOutputPipe = datapipe[0];
			/* write end of pipe is no longer required */
			close(datapipe[1]);
			command = command->next;
		}
		if (previousOutputPipe != inputFD)
			close(previousOutputPipe);
		dup2(inputFD, 1);
		dup2(outputFD, 0);
	}
	return 0;
}


/********************************************************************************
 * Function name  : int destroyCommand(struct command_s *command)
 *         command   : command structure to destroy by freeing all allocated
 *                     memory within it
 *
 * Created by     : James Johns
 * Date created   : 10/12/2011
 * Description    : Free all memory allocated to the structure by other command
 *                  functions such as interpretCommand()
 *
 * NOTES          : 
 ********************************************************************************/
void destroyCommand(struct command_s *command) {
	
	for (int i = 0; i < command->argc; i++){
		free(command->argv[i]);
	}
	free(command->argv);
	free(command);
}

