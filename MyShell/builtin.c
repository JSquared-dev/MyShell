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


void pwd() {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	getwd(path);
	printf("%s\n", path);
}

void cd(char *directory) {
	chdir(directory);
}
