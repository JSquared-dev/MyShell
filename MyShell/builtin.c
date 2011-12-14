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

void pwd() {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	getwd(path);
	printf("%s\n", path);
	free(path);
}

void cd(char *directory) {
	/* if we are not given a directory, return to home directory */
	if (directory == NULL || strcmp(directory, "") == 0)
		chdir(BASEHOMEDIRECTORY);
	else
		chdir(directory);
}
