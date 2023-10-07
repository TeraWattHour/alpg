EXTENSION = alpg

MODULE_big = alpg
DATA = alpg--0.0.1.sql
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

PG_CFLAGS = -I/usr/local/include -L/usr/local/lib
SHLIB_LINK = -lcurl -lcjson

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
