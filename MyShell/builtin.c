//
//  builtin.c
//  MyShell
//
//  Created by James Johns on 10/12/2011.
//  Copyright 2011 xD-Duncesoft. All rights reserved.
//

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
 * Function name  : void pwd()                                                  *
 *                                                                              *
 * Created by     : James Johns                                                 *
 * Date created   : 14/12/2011                                                  *
 * Description    : Print the current working directory to stdout               *
 *                                                                              *
 * NOTES          :                                                             *
 ********************************************************************************/
void pwd() {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	getwd(path);
	printf("%s\n", path);
	free(path);
}

/********************************************************************************
 * Function name  : void cd(char *directory)                                    *
 *         directory  : Path to new working directory.                          *
 *                                                                              *
 * Created by     : James Johns                                                 *
 * Date created   : 14/12/2011                                                  *
 * Description    : Change the current working directory to the specified       *
 *                  directory.                                                  *
 *                                                                              *
 * NOTES          : TODO - Implement home directory discovery for automatic     *
 *                         navigation to true home directory instead of place   *
 *                         holder home directory                                *
 ********************************************************************************/
void cd(char *directory) {
	/* if we are not given a directory, return to home directory */
	if (directory == NULL || strcmp(directory, "") == 0)
		chdir(BASEHOMEDIRECTORY);
	else
		chdir(directory);
}
