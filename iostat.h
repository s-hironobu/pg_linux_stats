/*-------------------------------------------------------------------------
 *
 * iostat.h
 *		Show iostat info on Linux
 *
 * Copyright (c) 2008-2024, PostgreSQL Global Development Group
 * Copyright (c) 2024, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#ifndef __IOSTAT_H__
#define __IOSTAT_H__

/*
$ iostat
Linux 5.15.0-94-generic (ubuntu-jammy) 	03/27/24 	_x86_64_	(2 CPU)

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
           0.15    0.00    0.12    0.01    0.00   99.72

Device             tps    kB_read/s    kB_wrtn/s    kB_dscd/s    kB_read    kB_wrtn    kB_dscd
loop0             0.00         0.00         0.00         0.00        397          0          0
loop1             0.00         0.01         0.00         0.00      44412          0          0
loop2             0.01         0.23         0.00         0.00     716922          0          0
loop3             0.00         0.00         0.00         0.00       1173          0          0
loop4             0.00         0.13         0.00         0.00     409440          0          0
loop5             0.00         0.10         0.00         0.00     298739          0          0
loop6             0.00         0.00         0.00         0.00       1182          0          0
loop7             0.00         0.00         0.00         0.00         97          0          0
loop8             0.00         0.00         0.00         0.00        111          0          0
sda               1.75        27.94        27.31         0.00   86603577   84648397          0
sdb               0.00         0.00         0.00         0.00       3226          0          0
*/  

typedef struct IoStat
{
	char		device[16];
	float4		tps;
	float4		kb_read_s;
	float4		kb_wrtn_s;
	float4		kb_dscd_s;
	int64		kb_read;
	int64		kb_wrtn;
	int64		kb_dscd;
}			IoStat;


extern List *get_iostat(List *iostat);

#endif
