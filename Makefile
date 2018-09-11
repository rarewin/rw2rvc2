CFLAGS =  -Wall -Wextra -Wswitch-enum -Wswitch-enum
CFLAGS += -MD

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0 -DDEBUG
else
	CFLAGS += -O2
endif

all: rw2rvc2

rw2rvc2: $(OBJS)

-include $(DEPS)

.PHONY: clean
clean:
	rm -f rw2rvc2
	rm -f $(OBJS)
	rm -f $(DEPS)

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: test
test: TEMP := $(shell tempfile)
test: rebuild
	@./tools/test.sh 10 10
	@./tools/test.sh 0 0
	@./tools/test.sh 255 255
	@./tools/test.sh 1+1 2
	@./tools/test.sh 100+1 101
	@./tools/test.sh 3-1 2
	@./tools/test.sh 255-1 254
	@./tools/test.sh "255 - 1" 254
	@./tools/test.sh "254 + 1" 255
	@./tools/test.sh "64 + 64 + 64" 192
	@./tools/test.sh "64 - 64 + 64 - 64" 0
	@./tools/test.sh "1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10" 55
	@./tools/test.sh "1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0" 0
	@./tools/test.sh "1 * 2" 2
	@./tools/test.sh "1 + 1 * 2" 3
	@./tools/test.sh "4 * 3 + 1 * 2" 14
	@./tools/test.sh "4 / 2" 2
	@./tools/test.sh "5 / 2" 2
	@./tools/test.sh "4 * 3 / 4" 3
	@./tools/test.sh "10 / 3 * 3" 9
