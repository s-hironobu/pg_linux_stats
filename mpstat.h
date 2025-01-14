/*-------------------------------------------------------------------------
 *
 * mpstat.h
 *		Show mpstat info on Linux
 *
 * Copyright (c) 2008-2025, PostgreSQL Global Development Group
 * Copyright (c) 2024-2025, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#ifndef __MPSTAT_H__
#define __MPSTAT_H__

/*
$ mpstat
Linux 5.15.0-94-generic (ubuntu-jammy) 	03/27/24 	_x86_64_	(2 CPU)

23:00:16     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
23:00:16     all    0.15    0.00    0.10    0.01    0.00    0.02    0.00    0.00    0.00   99.72

$ mpstat -P ALL
Linux 5.15.0-94-generic (ubuntu-jammy) 	03/28/24 	_x86_64_	(2 CPU)

08:58:39     CPU    %usr   %nice    %sys %iowait    %irq   %soft  %steal  %guest  %gnice   %idle
08:58:39     all    0.15    0.01    0.10    0.01    0.00    0.02    0.00    0.00    0.00   99.72
08:58:39       0    0.16    0.00    0.11    0.01    0.00    0.01    0.00    0.00    0.00   99.71
08:58:39       1    0.14    0.01    0.09    0.00    0.00    0.03    0.00    0.00    0.00   99.73

*/

typedef struct MpStat
{
	char		cpu[8];
	float4		usr;
	float4		nice;
	float4		sys;
	float4		iowait;
	float4		irq;
	float4		soft;
	float4		steal;
	float4		guest;
	float4		gnice;
	float4		idle;

}			MpStat;


extern List *get_mpstat(List *mpstat);

#endif
