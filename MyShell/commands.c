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
		int i;
		/* isolate command from the input */
		for (i = 0; i < strlen(commandLine) && commandLine[i] != ' ' && 
			 commandLine[i] != '\n'; i++); /* count through characters until we reach the end 
											* of the string or a tokenising character */
		
		toRet->argv[0] = malloc(sizeof(char)*(i+1));
		toRet->argc++;
		strncpy(toRet->argv[0], commandLine, i);
		toRet->argv[0][i] = NULL;
		toRet->argv[1] = NULL; /* make sure even for single commands, argv[] ends in a NULL pointer */
		
		/* if we hit a new line, then there is no more command to interpret. otherwise iterate past
		 * command utility and start parsing  */
		if (commandLine[i] == '\n')
			return toRet;
		else
			i++;
		
		/* temporary argument storage. ready to copy into a perfect sized array later on */
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
			else if (commandLine[i] == '|') {
				/* create a new command to link to. */
				toRet->next = interpretCommand(&commandLine[i+1]);
				break;
			}
		}
		
		/* copy arguments into command structure */
		if (toRet->argc > 1) {
			toRet->argv = realloc(toRet->argv, sizeof(char *)*(toRet->argc+1));
			/* loop through tempStore, copying all strings across to argv */
			for (int j = 1; j < toRet->argc; j++) {
				toRet->argv[j] = malloc(sizeof(char)*strlen(tempStore[j]));
				strncpy(toRet->argv[j], tempStore[j], strlen(tempStore[j]));
				free(tempStore[j]);
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
		printf("Executing fork/exec for command: %s\n", command->argv[0]);
		int previousOutputPipe = inputFD;
		do {
			int datapipe[2];
			if (pipe(datapipe) == -1)
				perror("pipe");
			
			/* link the new input pipe to the old output pipe. */
			/* if end of command string, print final output. otherwise continue redirection */
			if (command->next == NULL) {
				/* close datapipe[0] and use stdout for process output */
				close (datapipe[0]);
				datapipe[0] = outputFD;
			}
			else {
				/* store the output pipe to link to the next input pipe. */
				close(previousOutputPipe);
				previousOutputPipe = datapipe[0];
			}
			
			executeExternalCommand(command->argc, command->argv, previousOutputPipe, datapipe[1]);
			close(datapipe[1]);
		} while (command->next != NULL && (command = command->next));
		if (previousOutputPipe != inputFD)
			close(previousOutputPipe);
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

