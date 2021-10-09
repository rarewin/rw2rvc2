CFLAGS =  -Wall -Wextra -Werror -std=gnu11
CFLAGS += -MD

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0 -DDEBUG
	OUTDIR = debug
else
	CFLAGS += -O2
	OUTDIR = release
endif

SRCDIR = src

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OUTDIR)/,$(SRCS:$(SRCDIR)/%.c=%.o))
DEPS = $(OBJS:.o=.d)
TARGET = rw2rvc2

vpath %.c $(SRCDIR)
vpath %.o $(OUTDIR)

all: $(OUTDIR)/$(TARGET)

$(OUTDIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OUTDIR)/%.o: %.c
	-@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: fmt
fmt:
	clang-format -i $(SRCS)

.PHONY: clean
clean:
	rm -rf release debug
	rm -rf doc/html

	$(MAKE) -C test clean

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: test
test: release/rw2rvc2
	$(MAKE) -C test clean
	$(MAKE) -C test

.PHONY: doc
doc:
	doxygen doc/Doxyfile

-include $(DEPS)
