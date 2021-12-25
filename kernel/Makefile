PRIFIX  =
AS	= nasm
CC	= $(PRIFIX)gcc -O2 -std=c17
LD 	= $(PRIFIX)ld
OBJCOPY = $(PRIFIX)objcopy

CFLAGS  = -c -g -Wall -I../include   \
          -mcmodel=large -fno-hosted -fno-stack-protector

OBJS	= $(patsubst %.s,%.o ,$(wildcard *.s)) \
          $(patsubst %.c,%.o ,$(wildcard *.c))

.s.o:
	nasm  -felf64 -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -o $@ $<

kernel.bin: $(OBJS) kernel.lds Makefile
	$(LD) -static -T kernel.lds -Map kernel.map -o kernel.dbg $(OBJS)
	$(OBJCOPY) -S -R bss -O binary kernel.dbg kernel.bin
