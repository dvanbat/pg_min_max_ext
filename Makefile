EXTENSION = min_max
MODULES = min_max
DATA = min_max--0.1.sql

PG_CPPFLAGS = -g -O2
SHLIB_LINK = # -lz -llz4

ifndef PG_CONFIG
	PG_CONFIG := pg_config
endif
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
