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

	$(MAKE) -C test clean

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: test
test: release/rw2rvc2
	$(MAKE) -C test
#	@./tools/test.sh "int main() {a = 2 + 4; return a;}" 6
#	@./tools/test.sh "int main() {b = 2 * 4; a = 2 + 4; return a;}" 6
#	@./tools/test.sh "int main() {b = 2 * 4; a = 2 + b; return a;}" 10
#	@./tools/test.sh "int main() {b = 2 * 4; a = 2 * b; return a;}" 16
#	@./tools/test.sh "int main() {b = 2 * 4; a = b * 2; return a;}" 16
#	@./tools/test.sh "int main() {c = 10; b = 2 * 4; a = b * 2 + c; return a;}" 26
#	@./tools/test.sh "int main() {c = 10; b = 2 * 4 * c; a = b * 2 + c; return a;}" 170
#	@./tools/test.sh "int main() {hoge = 299; return 0;}" 0
#	@./tools/test.sh "int main() {return (2 + 3);}" 5
#	@./tools/test.sh "int main() {return (2 + 3) * (1 + 2);}" 15
#	@./tools/test.sh "int main() {return (2 + 3) + 4 * 2;}" 13
#	@./tools/test.sh "int main() {return (2 + 3) / 2 ;}" 2
#	@./tools/test.sh "int main() {a = (2 * 3) + 4; return a;}" 10
#	@./tools/test.sh "int main() {a = 2; if (a) a = 1; else a = 3; return a;}" 1
#	@./tools/test.sh "int main() {a = 2; if (a) {a = 1; return a;} else a = 3; return a;}" 1
#	@./tools/test.sh "int main() {if (2 - 2) return 1; return 0;}" 0
#	@./tools/test.sh "int main() {a = 6; if (a % 3) return 1; return 0;}" 0
#	@./tools/test.sh "int main() {a = 8; if (a % 3) return 1; return 0;}" 1
#	@./tools/test.sh "int main() {a = 2; if (a - 2) return 1; return 0;}" 0
#	@./tools/test.sh "int main() {a = 2; if (a - 2) {a = 1; return a;} else {a = 3; return a;} return a;}" 3
#	@./tools/test.sh "int main() {a = 2; if (a - 2) {a = 1; return a;} else {a = 3; return a;} return a;}" 3
#
#
#	@./tools/test.sh "int main() {a = 5; a *= 2; return a;}" 10
#	@./tools/test.sh "int main() {a = 15; a /= 4; return a;}" 3
#	@./tools/test.sh "int main() {a = 15; a %= 4; return a;}" 3
#	@./tools/test.sh "int main() {a = 15; a += 4; return a;}" 19
#	@./tools/test.sh "int main() {a = 15; a -= 4; return a;}" 11
#	@./tools/test.sh "int main() {a = 15; b = 2; a *= b; b -= a; a += (a + b); return a;}" 32
#
#	@./tools/test.sh "int main() {if (1 || 0) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (0 || 0) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (1 && 0) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (0 && 0) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (1 && 1) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (1 && 1 || 0) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (0 || 1 && 1) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (1 && 0 || 1 && 1) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (0 | 1) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (0 | 0) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (1 ^ 1) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (0 ^ 1) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {if (0 & 1) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (2 & 1) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {if (17 & 16) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a == 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a != 15) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {a = 15; if (a >= 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a <= 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a < 15) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {a = 15; if (a < 15) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {a = 15; if (a < 16) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a > 2) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a > 20) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {a = 15; if (a >= 20) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {a = 15; if (a >= 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 15; if (a <= 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 5; if (a <= 15) {return 0;} else {return 1;} }" 0
#	@./tools/test.sh "int main() {a = 35; if (a <= 15) {return 0;} else {return 1;} }" 1
#	@./tools/test.sh "int main() {return (1 << 3);}" 8
#	@./tools/test.sh "int main() {return (8 >> 3);}" 1
#	@./tools/test.sh "int main() {return (1 >> 1);}" 0
#	@./tools/test.sh "int main() {return (0 << 4);}" 0
#	@./tools/test.sh "int main() {return (1 << 32);}" 0
#	@./tools/test.sh "int main() {return (1 | 1 << 4);}" 17
#	@./tools/test.sh "int hoge(int a) {return a;} int main() {return hoge(17);}" 17
#	@./tools/test.sh "int mul(int a, int b) {return a * b;} int main() {return mul(3, 2);}" 6
#	@./tools/test.sh "int mul(int a, int b) {return a * b;} int main() {return mul(10, 0);}" 0

