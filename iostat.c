/*-------------------------------------------------------------------------
 *
 * iostat.c
 *		Show iostat info on Linux
 *
 * Copyright (c) 2008-2025, PostgreSQL Global Development Group
 * Copyright (c) 2024-2025, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "nodes/pg_list.h"

#include "iostat.h"


List *
get_iostat(List *iostat)
{
	FILE	   *fp;
	char		buffer[256];
	char	   *cmd = "/usr/bin/iostat";
	bool		reading = false;

	if ((fp = popen(cmd, "r")) == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not execute \"%s\": ", cmd)));


	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		IoStat	   *is;

		if (reading == true && strlen(buffer) > 1)
		{
			is = (IoStat *) palloc0(sizeof(IoStat));

			if (sscanf(buffer, "%s %f %f %f %f %ld %ld %ld",
					   is->device,
					   &(is->tps),
					   &(is->kb_read_s),
					   &(is->kb_wrtn_s),
					   &(is->kb_dscd_s),
					   &(is->kb_read),
					   &(is->kb_wrtn),
					   &(is->kb_dscd)) > 8)
				ereport(ERROR,
						(errcode(ERRCODE_DATA_EXCEPTION),
						 errmsg("unexpected output format: \"%s\"", cmd),
						 errdetail("number of fields is not corresponding")));

			iostat = lappend(iostat, is);
		}

		if (strlen(buffer) == 0)
			continue;
		else if (strncmp(buffer, "Device", 6) == 0)
		{
			reading = true;
			continue;
		}

	}


	pclose(fp);

	return iostat;
}
