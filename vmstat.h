/*-------------------------------------------------------------------------
 *
 * vmstat.h
 *		Show vmstat info on Linux
 *
 * Copyright (c) 2008-2024, PostgreSQL Global Development Group
 * Copyright (c) 2024, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#ifndef __VMSTAT_H__
#define __VMSTAT_H__

/*
  $ vmstat
  procs -----------memory---------- ---swap-- -----io---- -system-- ------cpu-----
  r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
  1  0      0  67340  31860 626072    0    0    14    14    5    5  0  0 100  0  0
*/


typedef struct VmStat
{
	int64		r;
	int64		b;
	int64		swpd;
	int64		free;
	int64		buff;
	int64		cache;
	int64		si;
	int64		so;
	int64		bi;
	int64		bo;
	int64		in;
	int64		cs;
	int64		us;
	int64		sy;
	int64		id;
	int64		wa;
	int64		st;
}			VmStat;

extern bool get_vmstat(struct VmStat *vmstat);

#endif
