# SQLite benchmark

This repo contains simple benchmark for SQLite. It was written for 
testing SQLite application 
([lib-sqlite](https://github.com/unikraft/lib-sqlite)) on 
[Unikraft](https://github.com/unikraft/unikraft).

## Building

SQLite3 is included in the repository.

```sh
$ make
```

## Usage

```
$ ./sqlite-bench --help

Usage: ./sqlite-bench [commands]
-c  Skip creation of the database (default = no).
-i  Skip INSERT queries (default = no).
-s  Skip SELECT queries (default = no).
-d  Skip DELETE queries (default = no).
-v  verbose mode (default = no).

```
