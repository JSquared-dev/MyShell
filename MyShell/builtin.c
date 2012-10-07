/****************************************************************************************
 * File name	: builtin.c
 *
 * Created by	: James Johns
 * Date created	: 10/12/2011
 *
 * Description	: Built-in functionality resides in here, such as pwd, ls, cd commands etc.
 *
 * NOTES		:	TODO - make functions accept file descriptors for input and output data
 *							to allow piping between commands.
 *					TODO - Find actual home directory for current user instead of 
 *							defaulting to /home/
 ****************************************************************************************/

#include "builtin.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#ifdef __APPLE__
	#define BASEHOMEDIRECTORY "/Users/"
#else
	#define BASEHOMEDIRECTORY "/home/"
#endif

/********************************************************************************
 * Function name  : void pwd(int argc, char **argv)
 *             argc  : Number of elements in argv.
 *             argv  : Array of NULL terminated strings.
 *
 * Created by     : James Johns
 * Date created   : 14/12/2011
 * Description    : Print the current working directory to stdout
 *
 * NOTES          : 
 ********************************************************************************/
void pwd(int argc, char **argv, int inputFD, int outputFD) {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	if (getwd(path) == NULL) {
	  perror("getwd");
	}
	else {
	  printf("%s\n", path);
	}
	free(path);
}

/********************************************************************************
 * Function name  : void cd(int argc, char **argv)
 *             argc  : Number of elements in argv.
 *             argv  : Array of NULL terminated strings.
 *
 * Created by     : James Johns
 * Date created   : 14/12/2011
 * Description    : Change the current working directory as specified by argv.
 *
 * NOTES          : TODO - Implement home directory discovery for automatic
 *                         navigation to true home directory instead of place
 *                         holder home directory
 ********************************************************************************/
void cd(int argc, char **argv, int inputFD, int outputFD) {
	/* if we are not given a directory, return to home directory */
	char *directory;
	if (argc > 1) {
		directory = argv[1];
	}
	else {
	  directory = getenv("HOME");
	}
	
	if (chdir(directory) != 0) 
		perror("cd");
}

/********************************************************************************
 * Function name  : void cexectureExternalCommand(int argc, char **argv, int inputFD, int outputFD)
 *				argc	: Number of elements in argv.
 *				argv	: Array of NULL terminated strings.
 *				inputFD : input pipe file descriptor. can be a valid pipe() fd or stdin.
 *				outputFD: output pipe file descriptor. can be a valid pipe() fd or stdout.
 *
 * Created by     : James Johns
 * Date created   : 1/10/2012
 * Description    : fork()s a new process, and executes the given command if found on the current 
 *						environment path.
 *
 * NOTES          : 
 ********************************************************************************/
void executeExternalCommand(int argc, char **argv, int inputFD, int outputFD) {
	int pid = fork();
	if (pid == 0) {
		/* child. setup pipes for redirecting input/output */
		inputFD = dup2(inputFD, 1);
		outputFD = dup2(outputFD, 0);
		/* execute command */
		execvp(argv[0], argv);
		perror("execvp");
		exit(1);
	}
	else if (pid > 0) {
		/* wait for process to end */
		waitpid(pid, NULL, 0);
	}
	else {
		perror("MyShell");
	}
}
