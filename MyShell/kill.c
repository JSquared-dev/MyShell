//
//  kill.c
//  MyShell
//
//  Created by James Johns on 04/12/2012.
//  Copyright 2012 xD-Duncesoft. All rights reserved.
//

#include "kill.h"
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

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
	int *pidlist = NULL; /* pid to signal */
	int signal = 0; /* signal to send to pid */
	if (argc == 2) {
		if (argv[1][0] != '-') { /* if first argument is not a flag, send SIGTERM to specified pid */
			pidlist = malloc(sizeof(int)*1);
			long inputPID = strtol(argv[1], NULL, 10);
			if (inputPID > INT_MAX || inputPID == 0) {
				const char *errormsg = "Invalid PID";
				write(outputFD, errormsg, strlen(errormsg));
				return;
			}
			else {
				pidlist[0] = (int)inputPID;
			}
			signal = SIGTERM;
		}
		else if (strcmp(argv[1], "-l") == 0) {
			write(outputFD, "print list of signals\n", 22);
			const char *signalsList = "1) SIGHUP\t\t9) SIGKILL\t\t15) SIGTERM\n";
			write(outputFD, signalsList, strlen(signalsList));
			return;
		  }
		  else {
		    perror("Invalid argument");
		  }
	}
	else if (argc >= 3) {
		/* first argument is signal to send */
		/* if the first character is a number, read the whole argument as a number */
		if (argv[1][0] == '-') {
		  if (sscanf(argv[1], "-%d", &signal) == 1) {
			/* ensure value is legal and will not discard overflowing bits unnecessarily */
			if (signal > INT_MAX || signal == 0) {
				const char *errormsg = "Invalid Signal";
				write(outputFD, errormsg, strlen(errormsg));
				return;
			}
		  }
		  else {
		    signal = 0;
		  }
		}
		else if (strcmp(argv[1], "SIGKILL") == 0) {
			signal = SIGKILL;
		}
		else if (strcmp(argv[1], "SIGHUP") == 0) {
			signal = SIGHUP;
		}
		else if (strcmp(argv[1], "SIGTERM") == 0) {
			signal = SIGTERM;
		}
		pidlist = malloc(sizeof(int)*(argc-2));
		/* second argument onwards are pids to signal */
		for (int i = 2; i < argc; i++) {
			long inputPID = strtol(argv[2], NULL, 10);
			/* ensure value is legal and will not discard overflowing bits unnecessarily */
			if (inputPID > INT_MAX || inputPID == 0) {
				const char *errormsg = "Invalid PID";
				write(outputFD, errormsg, strlen(errormsg));
				return;
			}
			else {
				pidlist[i-2] = (int)inputPID;
			}
		}
	}
	if (pidlist != NULL && signal != 0) {
		for (int i = 0; i < argc-2; i++) {
			if (kill(pidlist[i], signal) != 0) {
				perror("kill");
			}
		}
		free(pidlist);
	}
	else {
		const char *errormsg = "Error parsing arguments.\n";
		write(outputFD, errormsg, strlen(errormsg));
	}
}

