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
void pwd(int agrc, char **argv) {
	char *path = (char *)malloc(sizeof(char)*MAXPATHLENGTH);
	getwd(path);
	printf("%s\n", path);
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
void cd(int argc, char **argv) {
	/* if we are not given a directory, return to home directory */
	char *directory;
	if (argc != 0) {
		directory = argv[0];
	}
	else {
		directory = BASEHOMEDIRECTORY;
	}
	
	chdir(directory);
}
