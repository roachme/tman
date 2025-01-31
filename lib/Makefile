CC=gcc
AR=ar
CFLAGS=
LIBSTATIC=libtman.a
LIBDYNAMIC=libtman.so
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

.PHONY: $(LIBSTATIC) $(LIBDYNAMIC) clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(LIBSTATIC): $(OBJS)
	$(AR) -rc $@ $^

libclean:
	rm -rf $(LIBSTATIC) $(LIBDYNAMIC) $(OBJS)
