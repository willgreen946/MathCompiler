#ifndef __MATHCOMPILER_H__
#define __MATHCOMPILER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

enum { DIGITSTACKMAX = 128, OPERATORSTACKMAX = 64 };

int mcinit(void);

#endif /* __MATHCOMPILER_H__ */
