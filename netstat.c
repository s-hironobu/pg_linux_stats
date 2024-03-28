/*-------------------------------------------------------------------------
 *
 * netstat.c
 *		Show netstat info on Linux
 *
 * Copyright (c) 2008-2024, PostgreSQL Global Development Group
 * Copyright (c) 2024, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "nodes/pg_list.h"

#include "netstat.h"


List *
get_netstat(List *netstat)
{
	FILE	   *fp;
	char		buffer[256];
	char	   *cmd = "/usr/bin/netstat -i";

	if ((fp = popen(cmd, "r")) == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not execute \"%s\": ", cmd)));


	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		NetStat    *ns;

		if (strncmp(buffer, "Kernel", 6) == 0 || strncmp(buffer, "Iface", 5) == 0)
			continue;

		ns = (NetStat *) palloc0(sizeof(NetStat));

		if (sscanf(buffer, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld %s",
				   ns->iface,
				   &(ns->mtu),
				   &(ns->rx_ok),
				   &(ns->rx_err),
				   &(ns->rx_drp),
				   &(ns->rx_ovr),
				   &(ns->tx_ok),
				   &(ns->tx_err),
				   &(ns->tx_drp),
				   &(ns->tx_ovr),
				   ns->flg) > 11)
			ereport(ERROR,
					(errcode(ERRCODE_DATA_EXCEPTION),
					 errmsg("unexpected output format: \"%s\"", cmd),
					 errdetail("number of fields is not corresponding")));

		netstat = lappend(netstat, ns);

	}


	pclose(fp);

	return netstat;
}
