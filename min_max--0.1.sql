-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION min_max" to load this file. \quit

CREATE TABLE IF NOT EXISTS delimiter (
    d TEXT NOT NULL
);

CREATE OR REPLACE FUNCTION min_max_func(int[], integer)
	RETURNS int[]
    AS 'MODULE_PATHNAME'
    LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION min_max_result(int[])
    RETURNS text
    AS 'MODULE_PATHNAME'
    LANGUAGE C;

CREATE AGGREGATE min_max (int)
(
    sfunc = min_max_func,
    stype = int[],
    initcond = '{NULL,NULL}',
	finalfunc = min_max_result
);