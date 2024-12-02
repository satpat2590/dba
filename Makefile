CC=gcc
CFLAGS=-I. -I./src -fPIC -Wall -Wextra
LDFLAGS=-L./lib -ldl -lpthread -lm

# Step 3: Link and create executable
main: bin/main

bin/main: main.c build/sql.o lib/libsqlite3.so
	$(CC) $(CFLAGS) -o $@ main.c build/sql.o $(realpath lib/libsqlite3.so) $(LDFLAGS)

# Step 2: Compile SQL source to object file
sql_obj: build/sql.o

build/sql.o: src/sql.c src/sql.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Step 1: Build SQLite shared object
sqlite_so: lib/libsqlite3.so

lib/libsqlite3.so: lib/sqlite3.c lib/sqlite3.h
	$(CC) $(CFLAGS) -shared -o $@ $< -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS5

# Clean up
clean:
	rm -f lib/libsqlite3.so
	rm -rf build/*
	rm -rf bin/*

# Phony targets
.PHONY: sqlite_so sql_obj main clean