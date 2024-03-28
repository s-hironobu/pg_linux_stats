/*-------------------------------------------------------------------------
 *
 * vmstat.c
 *		Show vmstat info on Linux
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

#include "vmstat.h"

bool
get_vmstat(struct VmStat *vmstat)
{
	FILE	   *fp;
	char		buffer[256];
	char	   *cmd = "/usr/bin/vmstat";

	/* extract loadavg information */
	if ((fp = popen(cmd, "r")) == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not execute \"%s\": ", cmd)));

	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		if (strncmp(buffer, "procs", 5) == 0 || *buffer == 'r')
			continue;

		if (sscanf(buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
				   &(vmstat->r),
				   &(vmstat->b),
				   &(vmstat->swpd),
				   &(vmstat->free),
				   &(vmstat->buff),
				   &(vmstat->cache),
				   &(vmstat->si),
				   &(vmstat->so),
				   &(vmstat->bi),
				   &(vmstat->bo),
				   &(vmstat->in),
				   &(vmstat->cs),
				   &(vmstat->us),
				   &(vmstat->sy),
				   &(vmstat->id),
				   &(vmstat->wa),
				   &(vmstat->st)) > 18)
			ereport(ERROR,
					(errcode(ERRCODE_DATA_EXCEPTION),
					 errmsg("unexpected ouput format: \"%s\"", cmd),
					 errdetail("number of fields is not corresponding")));

	}

	pclose(fp);

	return true;
}
