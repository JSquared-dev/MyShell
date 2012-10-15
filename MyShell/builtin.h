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

void builtin_pwd(int argc, char **argv, int inputFD, int outputFD);
void builtin_cd(int argc, char **argv, int inputFD, int outputFD);
void builtin_kill(int argc, char **argv, int inputFD, int outputFD);

int forkAndExecute(int argc, char **argv, int inputFD, int outputFD);

#endif
