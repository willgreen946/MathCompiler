#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>

/*
 * Quits the program
 */
void cmdquit(const char *);

struct COMMANDMAP {
	const char * str;
	void (*fn) (const char *);
};

enum { cmdmapmax = 1 };

struct COMMANDMAP cmdmap[cmdmapmax];

#endif /* __COMMANDS_H__ */
