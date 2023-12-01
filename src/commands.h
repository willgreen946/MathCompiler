#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { cmdmapmax = 1 };

/*
 * Quits the program
 */
void cmdquit(const char *);

int iscommand(const char *);

struct COMMANDMAP {
	const char * str;
	void (*fn) (const char *);
};

#endif /* __COMMANDS_H__ */
