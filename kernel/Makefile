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

dep:
	sed -n 1,/\#\#\#Depend/p Makefile > makefile.tmp
	for file in $(wildcard *.c); do \
		$(CC) $(CFLAGS) -M $$file >> makefile.tmp; \
	done
	mv makefile.tmp Makefile

clean:
	-rm kernel.bin kernel.dbg kernel.map $(OBJS) *.o

###Depend
clock.o: clock.c ../include/cnix/kernel.h ../include/cnix/traps.h \
 ../include/cnix/asm.h ../include/cnix/desc.h
exceptions.o: exceptions.c ../include/cnix/kernel.h \
 ../include/cnix/traps.h ../include/cnix/asm.h
ioapic.o: ioapic.c ../include/cnix/kernel.h ../include/cnix/asm.h
lapic.o: lapic.c ../include/cnix/kernel.h ../include/cnix/asm.h
memory.o: memory.c ../include/cnix/kernel.h ../include/cnix/asm.h \
 ../include/cnix/desc.h ../include/cnix/traps.h
printk.o: printk.c ../include/cnix/kernel.h ../include/cnix/spinlock.h
sched.o: sched.c ../include/cnix/kernel.h ../include/cnix/sched.h \
 ../include/cnix/asm.h
smp.o: smp.c ../include/cnix/kernel.h
startup.o: startup.c ../include/cnix/kernel.h ../include/cnix/asm.h \
 ../include/cnix/desc.h ../include/cnix/traps.h
time.o: time.c ../include/cnix/kernel.h ../include/cnix/asm.h
vga.o: vga.c ../include/cnix/kernel.h ../include/cnix/asm.h
