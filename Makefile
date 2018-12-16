CFLAGS =  -Wall -Wextra
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
	rm -rf doc/html

	$(MAKE) -C test clean

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: test
test: release/rw2rvc2
	$(MAKE) -C test

.PHONY: doc
doc:
	doxygen doc/Doxyfile
