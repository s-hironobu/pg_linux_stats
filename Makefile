# pg_linux_stats/Makefile

MODULE_big = pg_linux_stats
OBJS = pg_linux_stats.o vmstat.o free.o netstat.o iostat.o mpstat.o

EXTENSION = pg_linux_stats
DATA = pg_linux_stats--1.0.sql

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_linux_stats
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif

