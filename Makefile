CFLAGS = -O2 -Wall -Wextra

all: rw2rvc2

rw2rvc2: rw2rvc2.c

.PHONY: clean
clean:
	rm rw2rvc2

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
