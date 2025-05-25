CC = gcc
CCFLAGS = -g -I. -Wall -Wextra -Wpedantic
LDFLAGS = -g -I.


SRCS = main.c logging.c token.c tokenizer.c
OBJS = $(SRCS:.c=.o)

outbin = loxc

.PHONY: all
all: $(outbin) $(OBJS)

$(outbin): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	-@rm -r $(outbin) $(OBJS)

