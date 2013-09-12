DEBUG=-g
CFLAGS=-Wall -Werror $(DEBUG)
LDFLAGS=-rdynamic
CC=gcc
STD=
OBJ=cpu_monitor.o \
		sys_monitor.o \
		cpu.o \
		parser.o \
		file_op.o \
		util.o

CPU_MONITOR=cpu_monitor

#.PHONY: all
#all:
#	$(TEST_CPU_MONITOR)
#	@echo ""
#	@echo "Hint: To run 'make test' is a good idea ;)"
#	@echo ""
#
#
#.PHONY: clean
#

$(CPU_MONITOR): $(OBJ)
	$(CC) $(LDFLAGS) $(STD) $(CFLAGS) -o cpu_monitor $(OBJ)

util.o: util.c util.h
	$(CC) $(STD) $(CFLAGS) -c util.c

file_op: file_op.c file_op.h
	$(CC) $(STD) $(CFLAGS) -c file_op.c

parser.o: parser.c parser.h util.h cpu.h
	$(CC) $(STD) $(CFLAGS) -c parser.c

cpu.o: cpu.c cpu.h parser.h
	$(CC) $(STD) $(CFLAGS) -c cpu.c

sys_monitor.o: sys_monitor.c sys_monitor.h cpu.h
	$(CC) $(STD) $(CFLAGS) -c sys_monitor.c

cpu_monitor.o: cpu_monitor.c sys_monitor.h
	$(CC) $(STD) $(CFLAGS) -c cpu_monitor.c

clean:
	rm $(CPU_MONITOR) $(OBJ)
