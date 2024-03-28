/*-------------------------------------------------------------------------
 *
 * pg_linux_stats.c
 *		run XXstat commands on Linux
 *
 * Copyright (c) 2008-2024, PostgreSQL Global Development Group
 * Copyright (c) 2024, Hironobu Suzuki @ interdb.jp
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "nodes/pg_list.h"
#include "utils/builtins.h"
#include "funcapi.h"
#include "tcop/utility.h"
#include "pgstat.h"

#include "vmstat.h"
#include "free.h"
#include "netstat.h"
#include "iostat.h"
#include "mpstat.h"


PG_MODULE_MAGIC;

/* Function declarations */
void		_PG_init(void);
void		_PG_fini(void);

Datum		pg_vmstat(PG_FUNCTION_ARGS);
Datum		pg_free(PG_FUNCTION_ARGS);
Datum		pg_netstat(PG_FUNCTION_ARGS);
Datum		pg_iostat(PG_FUNCTION_ARGS);
Datum		pg_mpstat(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pg_vmstat);
PG_FUNCTION_INFO_V1(pg_free);
PG_FUNCTION_INFO_V1(pg_netstat);
PG_FUNCTION_INFO_V1(pg_iostat);
PG_FUNCTION_INFO_V1(pg_mpstat);

/* Module callback */
void
_PG_init(void)
{
	if (!process_shared_preload_libraries_in_progress)
		return;

	EmitWarningsOnPlaceholders("pg_linux_stats");
}

void
_PG_fini(void)
{
	;
}

/*
 * Display vmstat
 */
#define NUM_VMSTAT_COLS		17

Datum
pg_vmstat(PG_FUNCTION_ARGS)
{
	TupleDesc	tupdesc;
	HeapTuple	tuple;
	Datum		values[NUM_VMSTAT_COLS];
	bool		nulls[NUM_VMSTAT_COLS];
	VmStat		vmstat;
	int			i = 0;

	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	Assert(tupdesc->natts == lengthof(values));

	get_vmstat(&vmstat);

	memset(nulls, 0, sizeof(nulls));
	memset(values, 0, sizeof(values));

	values[i++] = Int64GetDatum(vmstat.r);
	values[i++] = Int64GetDatum(vmstat.b);
	values[i++] = Int64GetDatum(vmstat.swpd);
	values[i++] = Int64GetDatum(vmstat.free);
	values[i++] = Int64GetDatum(vmstat.buff);
	values[i++] = Int64GetDatum(vmstat.cache);
	values[i++] = Int64GetDatum(vmstat.si);
	values[i++] = Int64GetDatum(vmstat.so);
	values[i++] = Int64GetDatum(vmstat.bi);
	values[i++] = Int64GetDatum(vmstat.bo);
	values[i++] = Int64GetDatum(vmstat.in);
	values[i++] = Int64GetDatum(vmstat.cs);
	values[i++] = Int64GetDatum(vmstat.us);
	values[i++] = Int64GetDatum(vmstat.sy);
	values[i++] = Int64GetDatum(vmstat.id);
	values[i++] = Int64GetDatum(vmstat.wa);
	values[i++] = Int64GetDatum(vmstat.st);

	tuple = heap_form_tuple(tupdesc, values, nulls);

	return HeapTupleGetDatum(tuple);
}


/*
 * Display free
 */
#define NUM_FREE_COLS		17

Datum
pg_free(PG_FUNCTION_ARGS)
{
	TupleDesc	tupdesc;
	HeapTuple	tuple;
	Datum		values[NUM_FREE_COLS];
	bool		nulls[NUM_FREE_COLS];
	Free		free;
	int			i = 0;

	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");

	Assert(tupdesc->natts == lengthof(values));

	get_free(&free);

	memset(nulls, 0, sizeof(nulls));
	memset(values, 0, sizeof(values));

	values[i++] = Int64GetDatum(free.total);
	values[i++] = Int64GetDatum(free.used);
	values[i++] = Int64GetDatum(free.free);
	values[i++] = Int64GetDatum(free.shared);
	values[i++] = Int64GetDatum(free.buff);
	values[i++] = Int64GetDatum(free.available);
	values[i++] = Int64GetDatum(free.swap_total);
	values[i++] = Int64GetDatum(free.swap_used);
	values[i++] = Int64GetDatum(free.swap_free);

	tuple = heap_form_tuple(tupdesc, values, nulls);

	return HeapTupleGetDatum(tuple);
}


/*
 * Display netstat -i
 */
#define NUM_NETSTAT_COLS 11

Datum
pg_netstat(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;
	Datum		values[NUM_NETSTAT_COLS];
	bool		nulls[NUM_NETSTAT_COLS];
	List	   *netstat = NIL;
	ListCell   *lc;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));


	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");
	Assert(tupdesc->natts == NUM_NETSTAT_COLS);

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	netstat = get_netstat(netstat);

	foreach(lc, netstat)
	{
		NetStat    *ns = (NetStat *) lfirst(lc);
		int			i;

		memset(values, 0, sizeof(values));
		memset(nulls, false, sizeof(nulls));

		i = 0;
		values[i++] = CStringGetTextDatum(ns->iface);
		values[i++] = Int64GetDatum(ns->mtu);
		values[i++] = Int64GetDatum(ns->rx_ok);
		values[i++] = Int64GetDatum(ns->rx_err);
		values[i++] = Int64GetDatum(ns->rx_drp);
		values[i++] = Int64GetDatum(ns->rx_ovr);
		values[i++] = Int64GetDatum(ns->tx_ok);
		values[i++] = Int64GetDatum(ns->tx_err);
		values[i++] = Int64GetDatum(ns->tx_drp);
		values[i++] = Int64GetDatum(ns->tx_ovr);
		values[i++] = CStringGetTextDatum(ns->flg);

		Assert(i == NUM_NETSTAT_COLS);
		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
		pfree(ns);
	}

	return (Datum) 0;
}

/*
 * Display iostat
 */
#define NUM_IOSTAT_COLS 8

Datum
pg_iostat(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;
	Datum		values[NUM_IOSTAT_COLS];
	bool		nulls[NUM_IOSTAT_COLS];
	List	   *iostat = NIL;
	ListCell   *lc;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));


	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");
	Assert(tupdesc->natts == NUM_IOSTAT_COLS);

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	iostat = get_iostat(iostat);

	foreach(lc, iostat)
	{
		IoStat	   *is = (IoStat *) lfirst(lc);
		int			i;

		memset(values, 0, sizeof(values));
		memset(nulls, false, sizeof(nulls));

		i = 0;
		values[i++] = CStringGetTextDatum(is->device);

		values[i++] = Float4GetDatum(is->tps);
		values[i++] = Float4GetDatum(is->kb_read_s);
		values[i++] = Float4GetDatum(is->kb_wrtn_s);
		values[i++] = Float4GetDatum(is->kb_dscd_s);

		values[i++] = Int64GetDatum(is->kb_read);
		values[i++] = Int64GetDatum(is->kb_wrtn);
		values[i++] = Int64GetDatum(is->kb_dscd);

		Assert(i == NUM_IOSTAT_COLS);
		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
		pfree(is);
	}

	return (Datum) 0;
}


/*
 * Display mpstat -P ALL
 */
#define NUM_MPSTAT_COLS 11

Datum
pg_mpstat(PG_FUNCTION_ARGS)
{
	ReturnSetInfo *rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	MemoryContext per_query_ctx;
	MemoryContext oldcontext;
	Datum		values[NUM_MPSTAT_COLS];
	bool		nulls[NUM_MPSTAT_COLS];
	List	   *mpstat = NIL;
	ListCell   *lc;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("materialize mode required, but it is not " \
						"allowed in this context")));


	/* Build a tuple descriptor for our result type */
	if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
		elog(ERROR, "return type must be a row type");
	Assert(tupdesc->natts == NUM_MPSTAT_COLS);

	/* Switch into long-lived context to construct returned data structures */
	per_query_ctx = rsinfo->econtext->ecxt_per_query_memory;
	oldcontext = MemoryContextSwitchTo(per_query_ctx);

	tupstore = tuplestore_begin_heap(true, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	mpstat = get_mpstat(mpstat);

	foreach(lc, mpstat)
	{
		MpStat	   *ms = (MpStat *) lfirst(lc);
		int			i;

		memset(values, 0, sizeof(values));
		memset(nulls, false, sizeof(nulls));

		i = 0;
		values[i++] = CStringGetTextDatum(ms->cpu);

		values[i++] = Float4GetDatum(ms->usr);
		values[i++] = Float4GetDatum(ms->nice);
		values[i++] = Float4GetDatum(ms->sys);
		values[i++] = Float4GetDatum(ms->iowait);
		values[i++] = Float4GetDatum(ms->irq);
		values[i++] = Float4GetDatum(ms->soft);
		values[i++] = Float4GetDatum(ms->steal);
		values[i++] = Float4GetDatum(ms->guest);
		values[i++] = Float4GetDatum(ms->gnice);
		values[i++] = Float4GetDatum(ms->idle);
		
		Assert(i == NUM_MPSTAT_COLS);
		tuplestore_putvalues(tupstore, tupdesc, values, nulls);
		pfree(ms);
	}

	return (Datum) 0;
}
