//
//  ps.h
//  MyShell
//
//  Created by James Johns on 02/12/2012.
//  Copyright 2012 xD-Duncesoft. All rights reserved.
//

#ifndef _PS_H_
#define _PS_H_

#include <stdio.h>
#include "builtin.h"

struct ps_s {
	int pid;
	char commandName[PATH_MAX];
	char state;
	int ppid, pgrp, session, tty_nr, tpgid;
	unsigned int flags;
	unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
	long cutime, cstime, priority, nice, num_threads, itrealvalue;
};

void builtin_ps(int argc, char **argv, int inputFD, int outputFD);
struct ps_s *parseStatFile(FILE *statFile);

#endif
