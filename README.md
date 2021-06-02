# PostgreSQL min_max extension
## Install

Build and install MIN_MAX:

```
cd /path/to/min_max/source/code
make
sudo make install
```

Connect to PostgreSQL:

```
psql <database_name>
```

Enable extension:

```
create extension min_max;
```

## Uninstall

Disable extension:

```
drop extension min_max cascade;
```

Uninstall:

```
cd /path/to/min_max/source/code
sudo make uninstall
```

## Usage
example
```
select min_max(val) from (VALUES(5),(3),(6),(7),(9),(10),(7),(4),(5002), (6000)) t(val);
----
result: 3 -> 6000
```

Default delimiter is '->'
To update delimiter insert row into table delimiter

example:
```
insert into delimiter values (' -- ');
select min_max(val) from (VALUES(5),(3),(6),(7),(9),(10),(7),(4),(5002), (6000)) t(val);
----
result: 3 -- 6000
```
