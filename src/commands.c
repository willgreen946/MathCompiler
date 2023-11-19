#include <stdio.h>
#include <stdlib.h>
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
