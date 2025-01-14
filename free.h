
/*-------------------------------------------------------------------------
 *
 * free.h
 *		Show free info on Linux
 *
 * Copyright (c) 2008-2025, PostgreSQL Global Development Group
 * Copyright (c) 2024-2025, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#ifndef __FREE_H__
#define __FREE_H__

/*
$ free
               total        used        free      shared  buff/cache   available
Mem:          980320      255048       67340       48176      657932      514884
Swap:              0           0           0
*/

typedef struct Free
{
	int64		total;
	int64		used;
	int64		free;
	int64		shared;
	int64		buff;
	int64		available;
	int64		swap_total;
	int64		swap_used;
	int64		swap_free;
}			Free;

extern bool get_free(struct Free *free);

#endif
