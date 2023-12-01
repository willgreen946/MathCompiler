#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

struct COMMANDMAP cmdmap[cmdmapmax] = {
	{ "quit", cmdquit },
};

void
cmdquit(const char * s)
{
	if (!s)
		exit(0);

	exit(0);
}

int
iscommand(const char * s)
{
	int i;

	for (i = 0; i < cmdmapmax; i++) {
		if (!strncmp(
			s, cmdmap[i].str, strlen(cmdmap[i].str))) {
			cmdmap[i].fn(s);
			return 1;
		}
	}

	return 0;
}
