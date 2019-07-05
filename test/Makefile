TEMP := $(shell tempfile)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

.PHONY: test
test: $(OBJS)
	@../tools/gen_test.py . > test.c
	@riscv64-linux-gnu-gcc test.c -c -o test.o
	@riscv64-linux-gnu-gcc -static *.o -o test
	@qemu-riscv64-static test
	@echo "\\e[1;32mAll green!!\\e[m\\n"

%.o: %.c
	@../release/rw2rvc2 -z $< > $(@:.o=.s)
	@riscv64-linux-gnu-gcc $(@:.o=.s) -c -o $@

.PHONY: clean
clean:
	$(RM) test test.c
	$(RM) *.o *.s

