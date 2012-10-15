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
#include <sys/types.h>
#include <signal.h>
#include <string.h>

/********************************************************************************
 * Function name  : void builtin_pwd(int argc, char **argv)
 *             argc  : Number of elements in argv.
 *             argv  : Array of NULL terminated strings.
 *			 inputFD : input pipe file descriptor. can be a valid pipe() fd or stdin.
 *		 	 outputFD: output pipe file descriptor. can be a valid pipe() fd or stdout.
 *
 * Created by     : James Johns
 * Date created   : 14/12/2011
 * Description    : Print the current working directory to stdout
 *
 * NOTES          : 
 ********************************************************************************/
void builtin_pwd(int argc, char **argv, int inputFD, int outputFD) {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	if (path == NULL) {
		perror("pwd");
		return;
	}
	write(outputFD, "BUILTIN\n", 8);
	if (getwd(path) == NULL) {
		perror("getwd");
	}
	else {
		write(outputFD, path, strlen(path));
	}
	free(path);
}

/********************************************************************************
 * Function name  : void builtin_cd(int argc, char **argv)
 *             argc  : Number of elements in argv.
 *             argv  : Array of NULL terminated strings.
 *			 inputFD : input pipe file descriptor. can be a valid pipe() fd or stdin.
 *			 outputFD: output pipe file descriptor. can be a valid pipe() fd or stdout.
 *
 * Created by     : James Johns
 * Date created   : 14/12/2011
 * Description    : Change the current working directory as specified by argv.
 *
 * NOTES          : TODO - Implement home directory discovery for automatic
 *                         navigation to true home directory instead of place
 *                         holder home directory
 ********************************************************************************/
void builtin_cd(int argc, char **argv, int inputFD, int outputFD) {
	/* if we are not given a directory, return to home directory */
	char *directory;
	write(outputFD, "BUILTIN\n", 8);
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
 * Function name  : void builtin_kill(int argc, char **argv)
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
void builtin_kill(int argc, char **argv, int inputFD, int outputFD) {
	int pid; /* pid to signal */
	int signal; /* signal to send to pid */
	if (argc == 2) {
		if (argv[1][0] != '-') { /* if first argument is not a flag, send SIGTERM to specified pid */
			pid = atoi(argv[1]);
			signal = SIGTERM;
		}
		else if (strcmp(argv[1], "-l") == 0) {
			write(outputFD, "print list of signals\n", 22);
			return;
		}
		else {
			write(outputFD, "Invalid argument\n", 17);
		}
	}
	else if (argc == 3) {
		/* first argument is signal to send */
		/* second argument is pid to signal */
	}
	if (kill(pid, signal) != 0) {
		perror("kill");
	}
}

/********************************************************************************
 * Function name  : int forkAndExecute(int argc, char **argv, int inputFD, int outputFD)
 *			returns	: 0 on success, positive on failure, negative on termination.
 *				argc	: Number of elements in argv.
 *				argv	: Array of NULL terminated strings.
 *				inputFD : input pipe file descriptor. can be a valid pipe() fd or stdin.
 *				outputFD: output pipe file descriptor. can be a valid pipe() fd or stdout.
 *
 * Created by     : James Johns
 * Date created   : 1/10/2012
 * Description    : if argv[0] is a command that requires a new process to execute, this fork()s 
 *                      a new process, and executes the given command if found on the current 
 *						environment path. 
 *	                    otherwise it executes the command as a part of the current process. e.g. CD
 *
 * NOTES          : 
 ********************************************************************************/
int forkAndExecute(int argc, char **argv, int inputFD, int outputFD) {
	/* cd and exit/quit are not supported by multi-processing. deal with them without fork()ing */
	if (strcmp(argv[0], "cd") == 0) {
		builtin_cd(argc, argv, inputFD, outputFD);
	}
	else if ((strcmp(argv[0], "quit") == 0) || (strcmp(argv[0], "exit") == 0)) {
		return -1;
	}
	else {
		int pid = fork();
		if (pid == 0) {
			/* child. setup pipes for redirecting input/output */
			inputFD = dup2(inputFD, 0);
			outputFD = dup2(outputFD, 1);
			/* execute command */
			if (strcmp(argv[0], "pwd") == 0) {
				builtin_pwd(argc, argv, inputFD, outputFD);
			}
			else if (strcmp(argv[0], "kill") == 0) {
				builtin_kill(argc, argv, inputFD, outputFD);
			}
			else {
				execvp(argv[0], argv);
				perror("execvp");
				exit(1);
			}
			exit(0);
		}
		else if (pid > 0) {
			/* wait for process to end */
			waitpid(pid, NULL, 0);
		}
		else {
			perror("MyShell");
		}
	}
	return 0;
}
