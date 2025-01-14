/*-------------------------------------------------------------------------
 *
 * netstat.h
 *		Show netstat info on Linux
 *
 * Copyright (c) 2008-2025, PostgreSQL Global Development Group
 * Copyright (c) 2024-2025, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#ifndef __NETSTAT_H__
#define __NETSTAT_H__

/*
$ netstat -i
Kernel Interface table
Iface      MTU    RX-OK RX-ERR RX-DRP RX-OVR    TX-OK TX-ERR TX-DRP TX-OVR Flg
enp0s3    1500  1180782      0      0 0        219844      0      0      0 BMRU
enp0s8    1500    19953      0      0 0           883      0      0      0 BMRU
enp0s9    1500  2745270      0      0 0       1862516      0      0      0 BMRU
lo       65536     2791      0      0 0          2791      0      0      0 LRU
*/

typedef struct NetStat
{
	char		iface[16];
	int64		mtu;
	int64		rx_ok;
	int64		rx_err;
	int64		rx_drp;
	int64		rx_ovr;
	int64		tx_ok;
	int64		tx_err;
	int64		tx_drp;
	int64		tx_ovr;
	char		flg[16];

}			NetStat;


extern List *get_netstat(List *netstat);

#endif
