/*-------------------------------------------------------------------------
 *
 * mpstat.c
 *		Show mpstat info on Linux
 *
 * Copyright (c) 2008-2025, PostgreSQL Global Development Group
 * Copyright (c) 2024-2025, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "nodes/pg_list.h"

#include "mpstat.h"


List *
get_mpstat(List *mpstat)
{
	FILE	   *fp;
	char		buffer[256];
	char	   *cmd = "/usr/bin/mpstat -P ALL";
	bool		reading = false;

	if ((fp = popen(cmd, "r")) == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not execute \"%s\": ", cmd)));


	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		MpStat	   *ms;
		char		dummy1[16];
		char		dummy2[16];

		if (reading == true)
		{
			ms = (MpStat *) palloc0(sizeof(MpStat));

			if (sscanf(buffer, "%s %s %f %f %f %f %f %f %f %f %f %f",
					   dummy1,
					   ms->cpu,
					   &(ms->usr),
					   &(ms->nice),
					   &(ms->sys),
					   &(ms->iowait),
					   &(ms->irq),
					   &(ms->soft),
					   &(ms->steal),
					   &(ms->guest),
					   &(ms->gnice),
					   &(ms->idle)) > 12)
				ereport(ERROR,
						(errcode(ERRCODE_DATA_EXCEPTION),
						 errmsg("unexpected output format: \"%s\"", cmd),
						 errdetail("number of fields is not corresponding")));

			mpstat = lappend(mpstat, ms);
		}
		else					/* reading == false */
		{
			/*--
			 * strlen("CPU %usr %nice %sys %iowait %irq %soft %steal %guest %gnice %idle") = 66
			 */
			if (strlen(buffer) < 66)
				continue;
			if (sscanf(buffer, "%s %s", dummy1, dummy2) < 2)
				continue;
			if (strncmp(dummy2, "CPU", 3) == 0)
				reading = true;
		}
	}


	pclose(fp);

	return mpstat;
}
