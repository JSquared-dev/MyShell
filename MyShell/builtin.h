/****************************************************************************************
 * File name	: builtin.h
 *
 * Created by	: James Johns
 * Date created	: 10/12/2011
 *
 * Description	: Built-in functionality resides in here, such as pwd, ls, cd commands etc.
 *
 * NOTES		:	TODO - Create ls and ps commands.
 *					TODO - Create fork/exec command for running non-built-in commands.
 ****************************************************************************************/

#ifndef _BUILTIN_H_
#define _BIULTIN_H_

#define MAXPATHLENGTH 1024

void pwd(int argc, char **argv);
void cd(int argc, char **argv);

#endif
