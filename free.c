/*-------------------------------------------------------------------------
 *
 * free.c
 *		Show free info on Linux
 *
 * Copyright (c) 2008-2024, PostgreSQL Global Development Group
 * Copyright (c) 2024, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "free.h"

bool
get_free(struct Free *free)
{
	FILE	   *fp;
	char		buffer[256];
	char	   *cmd = "/usr/bin/free";

	if ((fp = popen(cmd, "r")) == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not execute \"%s\": ", cmd)));

	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		if (strncmp(buffer, "Mem:", 4) == 0)
		{
			if (sscanf(buffer, "Mem: %ld %ld %ld %ld %ld %ld",
					   &(free->total),
					   &(free->used),
					   &(free->free),
					   &(free->shared),
					   &(free->buff),
					   &(free->available)) > 6)
				ereport(ERROR,
						(errcode(ERRCODE_DATA_EXCEPTION),
						 errmsg("unexpected ouput format: \"%s\"", cmd),
						 errdetail("number of fields is not corresponding")));
		}
		else if (strncmp(buffer, "Swap:", 5) == 0)
		{
			if (sscanf(buffer, "Swap: %ld %ld %ld",
					   &(free->swap_total),
					   &(free->swap_used),
					   &(free->swap_free)) > 3)
				ereport(ERROR,
						(errcode(ERRCODE_DATA_EXCEPTION),
						 errmsg("unexpected ouput format: \"%s\"", cmd),
						 errdetail("number of fields is not corresponding")));
		}
	}

	pclose(fp);

	return true;
}
