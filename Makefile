CC=gcc
CFLAGS=-I. -fPIC
DEPS=sqlite3.h
OBJ=main.o sqlite3.o 
LIBS=-ldl -lpthread -lm

main: $(OBJ) libsqlite3.so
	$(CC) -o $@ $(OBJ) -L. $(LIBS)
	
sqlite3.o: sqlite3.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS5

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libsqlite3.so: sqlite3.o
	$(CC) -shared -o $@ $< $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core *~ libsqlite3.so main