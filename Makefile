SRC=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRC))
CFLAGS=-O2

.PHONY: all
all: chlottery.out

.PHONY: clean
clean:
	rm $(OBJS)
	rm chlottery.out

.PHONY: run
run:
	chlottery.out

.PHONY: install
install: chlottery.out
	mv chlottery.out /usr/bin/chlottery

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

chlottery.out: $(OBJS)
	$(CC) -o chlottery.out *.o $(CFLAGS) $(LNFLAGS)
