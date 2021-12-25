PRIFIX  =
AS	= nasm
CC	= $(PRIFIX)gcc -O2 -std=c17
LD 	= $(PRIFIX)ld
OBJCOPY = $(PRIFIX)objcopy

CFLAGS  = -c -g -Wall -I../include  \
          -mcmodel=large -fno-hosted -fno-stack-protector


QEMU    = /opt/qemu/qemu-system-x86_64 -L /opt/qemu/ \
          -monitor stdio -display sdl
IMAGE   = cnix-fda.img

.s.o:
	nasm -g -felf64 -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -o $@ $<

run: $(IMAGE)
	$(QEMU) -fda $(IMAGE)

$(IMAGE): boot.bin kernel.bin Makefile
	[ -f $(IMAGE) ] || dd if=/dev/zero of=$(IMAGE) count=$$((80*18*2))
	dd if=boot.bin of=$(IMAGE) count=1 conv=notrunc
	dd if=kernel.bin of=$(IMAGE) count=80 seek=1 conv=notrunc

boot.bin: boot.s
	nasm -f bin -o boot.bin boot.s

kernel.bin: kernel.o
	$(LD) -static -T kernel.lds -Map kernel.map -o kernel.dbg kernel.o
	$(OBJCOPY) -S -R bss -O binary kernel.dbg kernel.bin
