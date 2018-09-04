CFLAGS = -O2 -Wall -Wextra

all: rw2rvc2

rw2rvc2: rw2rvc2.c

.PHONY: clean
clean:
	rm rw2rvc2

.PHONY: test
test: TEMP := $(shell tempfile)
test: rw2rvc2
	@./rw2rvc2 10 > $(TEMP).s
	@riscv64-linux-gnu-gcc -static $(TEMP).s -o $(TEMP)
	@$(TEMP) || if [ "$$?" != "10" ]; then echo "NG"; else echo "OK"; fi

	@./rw2rvc2 0 > $(TEMP).s
	@riscv64-linux-gnu-gcc -static $(TEMP).s -o $(TEMP)
	@$(TEMP) && if [ "$$?" != "0" ]; then echo "NG"; else echo "OK"; fi

	@./rw2rvc2 255 > $(TEMP).s
	@riscv64-linux-gnu-gcc -static $(TEMP).s -o $(TEMP)
	@$(TEMP) || if [ "$$?" != "255" ]; then echo "NG"; else echo "OK"; fi
