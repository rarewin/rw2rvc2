CFLAGS = -Wall -Wextra

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0
else
	CFLAGS += -O2
endif

all: rw2rvc2

rw2rvc2: rw2rvc2.c

.PHONY: clean
clean:
	rm -f rw2rvc2

.PHONY: test
test: TEMP := $(shell tempfile)
test: rw2rvc2
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
