CFLAGS =  -Wall -Wextra -Wswitch-enum -Wswitch-enum
CFLAGS += -MD

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0 -DDEBUG
	OUTDIR = debug
else
	CFLAGS += -O2
	OUTDIR = release
endif

SRCS = $(wildcard src/*.c)
OBJS = $(addprefix $(OUTDIR)/,$(subst src/,,$(SRCS:.c=.o)))
DEPS = $(OBJS:.o=.d)
TARGET = $(OUTDIR)/rw2rvc2

vpath %.c src

all: $(TARGET)

$(OUTDIR):
	mkdir $(OUTDIR)

$(TARGET): $(OBJS)

-include $(DEPS)

$(OUTDIR)/%.o: $(OUTDIR) %.c
	$(CC) $(CFLAGS) -c $(word 2,$^) -o $@

.PHONY: clean
clean:
	rm -f rw2rvc2
	rm -rf release debug

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: test
test: TEMP := $(shell tempfile)
test: rebuild
	@./tools/test.sh "return 10;" 10
	@./tools/test.sh "return 0;" 0
	@./tools/test.sh "return 255;" 255
	@./tools/test.sh "return 1+1;" 2
	@./tools/test.sh "return 100+1;" 101
	@./tools/test.sh "return 3-1;" 2
	@./tools/test.sh "return 255-1;" 254
	@./tools/test.sh "return 255 - 1;" 254
	@./tools/test.sh "return 254 + 1;" 255
	@./tools/test.sh "return 64 + 64 + 64;" 192
	@./tools/test.sh "return 64 - 64 + 64 - 64;" 0
	@./tools/test.sh "return 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;" 55
	@./tools/test.sh "return 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0;" 0
	@./tools/test.sh "return 1 * 2;" 2
	@./tools/test.sh "return 1 + 1 * 2;" 3
	@./tools/test.sh "return 4 * 3 + 1 * 2;" 14
	@./tools/test.sh "return 4 / 2;" 2
	@./tools/test.sh "return 5 / 2;" 2
	@./tools/test.sh "return 4 * 3 / 4;" 3
	@./tools/test.sh "return 10 / 3 * 3;" 9
	@./tools/test.sh "1 + 2; return 10 / 3 * 3;" 9
	@./tools/test.sh "1 + 2; 1 + 2; 1 + 2; 1 + 2; 1 + 2; 1 + 2; 1 + 2; return 10 / 3 * 3;" 9
	@./tools/test.sh "1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0; 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 - 10 - 9 - 8 - 7 - 6 - 5 - 4 - 3 - 2 - 1 - 0 + 0; return 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;" 55
	@./tools/test.sh "a = 2; return a;" 2
	@./tools/test.sh "a = 2 + 4; return a;" 6
	@./tools/test.sh "b = 2 * 4; a = 2 + 4; return a;" 6
	@./tools/test.sh "b = 2 * 4; a = 2 + b; return a;" 10
	@./tools/test.sh "b = 2 * 4; a = 2 * b; return a;" 16
	@./tools/test.sh "b = 2 * 4; a = b * 2; return a;" 16
	@./tools/test.sh "c = 10; b = 2 * 4; a = b * 2 + c; return a;" 26
	@./tools/test.sh "c = 10; b = 2 * 4 * c; a = b * 2 + c; return a;" 170
	@./tools/test.sh "return (2 + 3);" 5
	@./tools/test.sh "return (2 + 3) * (1 + 2);" 15
	@./tools/test.sh "return (2 + 3) + 4 * 2;" 13
	@./tools/test.sh "return (2 + 3) / 2 ;" 2
	@./tools/test.sh "a = (2 * 3) + 4; return a;" 10
	@./tools/test.sh "if (1) return 1; return 0;" 1
	@./tools/test.sh "if (0) return 1; return 0;" 0
	@./tools/test.sh "if (a = 2) return 1; return 0;" 1

	@./tools/fail.sh "c = ="
	@./tools/fail.sh "return 1"
	@./tools/fail.sh "return a;"
	@./tools/fail.sh "b = 2 * a; return b;"
	@./tools/fail.sh "a = 2 * a; return a;"
