/* pg_linux_stats--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_linux_stats" to load this file. \quit

-- Register functions.


CREATE OR REPLACE FUNCTION pg_vmstat(
       OUT r int,
       OUT b int,
       OUT swpd int,
       OUT free int,
       OUT buff int,
       OUT cache int,
       OUT si int,
       OUT so int,
       OUT bi int,
       OUT bo int,
       OUT interrupts int,
       OUT cs int,
       OUT us int,
       OUT sy int,
       OUT id int,
       OUT wa int,
       OUT st int
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pg_free(
       OUT total int,
       OUT used int,
       OUT free int,
       OUT shared int,
       OUT buff int,
       OUT available int,
       OUT swap_total int,
       OUT swap_used int,
       OUT swap_free int
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION pg_netstat(
       OUT iface text,
       OUT mtu int,
       OUT rx_ok int,
       OUT rx_err int,
       OUT rx_drp int,
       OUT rx_ovr int,
       OUT tx_ok int,
       OUT tx_err int,
       OUT tx_drp int,
       OUT tx_ovr int,
       OUT flg text
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION pg_iostat(
       OUT device text,
       OUT tps real,
       OUT kb_read_s real,
       OUT kb_wrtn_s real,
       OUT kb_dscd_s real,
       OUT kb_read int,
       OUT kb_wrtn int,
       OUT kb_dscd int
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


CREATE OR REPLACE FUNCTION pg_mpstat(
       OUT cpu text,
       OUT usr real,
       OUT nice real,
       OUT sys real,
       OUT iowait real,
       OUT irq real,
       OUT soft real,
       OUT steal real,
       OUT guest real,
       OUT gnice real,
       OUT idle real
)
RETURNS SETOF record
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;


-- GRANT SELECT ON pg_show_vm TO PUBLIC;

-- Don't want this to be available to non-superusers.
--REVOKE ALL ON FUNCTION pg_show_vm() FROM PUBLIC;
