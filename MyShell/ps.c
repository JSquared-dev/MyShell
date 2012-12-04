//
//  ps.c
//  MyShell
//
//  Created by James Johns on 02/12/2012.
//  Copyright 2012 xD-Duncesoft. All rights reserved.
//

#include "ps.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>


/********************************************************************************
 * Function name  : void builtin_ps(int argc, char **argv, int inputFD, int outputFD)
 *             argc  : Number of elements in argv.
 *             argv  : Array of NULL terminated strings.
 *			 inputFD : input pipe file descriptor. can be a valid pipe() fd or stdin.
 *			 outputFD: output pipe file descriptor. can be a valid pipe() fd or stdout.
 *
 * Created by     : James Johns
 * Date created   : 2/12/2012
 * Description    : Print process information to outputFD.
 *
 *					No arguments specified - print the stats of the current pid and any child 
 *												processes currently running
 *
 * NOTES          : 
 ********************************************************************************/
void builtin_ps(int argc, char **argv, int inputFD, int outputFD) {
	FILE *output = fdopen(dup(outputFD), "w");
	FILE *input = fdopen(dup(inputFD), "r");
	struct ps_s *statContent;
	int hours, mins, secs;
	char *procFileName = malloc(sizeof(char)*PATH_MAX); /* allocate excess in case of long file structures */
	for (int i = 0; i < PATH_MAX;i++, procFileName[i] = '\0');
	
	fprintf(output, "BUILTIN\n");	
	if (argc == 1) {
		int cpid = getpid(); /* current PID */
		sprintf(procFileName, "/proc/%d/stat", cpid);
		FILE *statFile = fopen(procFileName, "r");
		if (statFile == NULL) {
			perror("fopen");
			return;
		}
		sprintf(procFileName, "/proc/%d/task", cpid);
		DIR *taskDir = opendir(procFileName);
		if (taskDir == NULL) {
			perror("opendir");
			return;
		}
		
		fprintf(output, "  PID\t TTY\t TIME\t COMMAND\n");
		/* print initial pid before descending through child processes */
		statContent = parseStatFile(statFile);
		fprintf(output, " %d\t %s\t %02d:%02d:%02d  %s\n", statContent->pid, statContent->ttyDeviceName,
				statContent->hours, statContent->mins, statContent->secs, statContent->commandName);
		fclose(statFile);
		
		/* loop through every directory entry and get child pids from them
		 * then print their stat files */
		struct dirent *curDirent;
		while ((curDirent = readdir(taskDir)) != NULL) {
			int childPID;
			if (sscanf(curDirent->d_name, "%d", &childPID) == 1 && childPID != cpid) {
				sprintf(procFileName, "/proc/%d/stat", childPID);
				statFile = fopen(procFileName, "r");
				if (statFile == NULL) {
					perror("fopen");
					continue;
				}
				statContent = parseStatFile(statFile);
				fprintf(output, " %d\t %s\t %02d:%02d:%02d  %s\n", statContent->pid, 
						statContent->ttyDeviceName, statContent->hours, statContent->mins, 
						statContent->secs, statContent->commandName);
				fclose (statFile);
			}
		}
		closedir(taskDir);
	}
	else if (argc == 2) {
		if (strcmp(argv[1], "-A") == 0) {	/* list every running process */
			fprintf(output, "  PID TTY\t TIME\t  COMMAND\n");
			/* loop through every directory entry in /proc/ and get child pids from them
			 * then print their stat files */
			DIR *taskDir = opendir("/proc");
			FILE *statFile = NULL;
			struct dirent *curDirent;
			while ((curDirent = readdir(taskDir)) != NULL) {
				int childPID;
				if (sscanf(curDirent->d_name, "%d", &childPID) == 1) {
					sprintf(procFileName, "/proc/%d/stat", childPID);
					statFile = fopen(procFileName, "r");
					if (statFile == NULL) {
						perror("fopen");
						continue;
					}
					statContent = parseStatFile(statFile);
					fprintf(output, " %d\t %s\t %02d:%02d:%02d  %s\n", statContent->pid, 
							statContent->ttyDeviceName, statContent->hours, statContent->mins, 
							statContent->secs, statContent->commandName);
					fclose (statFile);
				}
			}
			closedir(taskDir);
		}
		else {
			fprintf(output, "Did not understand flag: %s", argv[1]);
		}
	}
	else {
		fprintf(output, "Too many arguments");
	}
	fflush(output);
	fclose(input);
	fclose(output);
}


/********************************************************************************
 * Function name  : struct ps_s *parseStatFile(FILE *statFile)
 *		return : pointer to filled ps_s structure containing all data held in specified statFile.
 *          statFile : Open file stream of the stat file in a pid's /proc folder.
 *
 * Created by     : James Johns
 * Date created   : 2/12/2012
 * Description    : parse statFile and return a pointer to the structure created.
 *
 * NOTES          : 
 ********************************************************************************/
struct ps_s *parseStatFile(FILE *statFile) {
	struct ps_s *toRet = malloc(sizeof(struct ps_s));
	
	fscanf(statFile, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %l %l %l %l %l %l",
		   &toRet->pid, &toRet->commandName, &toRet->state, &toRet->ppid, &toRet->pgrp, 
		   &toRet->session, &toRet->tty_nr, &toRet->tpgid, &toRet->flags, &toRet->minflt, 
		   &toRet->cminflt, &toRet->majflt, &toRet->cmajflt, &toRet->utime, &toRet->stime, 
		   &toRet->cutime, &toRet->cstime, &toRet->priority, &toRet->nice, &toRet->num_threads, 
		   &toRet->itrealvalue);
	
	toRet->hours = (int) ((toRet->stime+toRet->utime))/360000;
	toRet->mins = (int) ((toRet->stime+toRet->utime)%360000)/6000;
	toRet->secs = (int) ((toRet->stime+toRet->utime)%6000)/100;
	
	char *devicePrefix = ((toRet->tty_nr & 0xFF00) >> 8) == 0x88 ? "pts/" : "/dev/tty";
	int deviceNumber = (toRet->tty_nr & 0xFF) | ((toRet->tty_nr & 0xFFFF0000) >> 16);
	sprintf((char *)toRet->ttyDeviceName, "%.46s%4d", devicePrefix, deviceNumber);

	return toRet;
}
