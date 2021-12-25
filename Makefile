QEMU    = /opt/qemu/qemu-system-x86_64 -L /opt/qemu/ \
          -monitor stdio -display sdl
IMAGE   = cnix-fda.img

run: $(IMAGE)
	$(QEMU) -fda $(IMAGE)

$(IMAGE): boot/boot.bin kernel/kernel.bin Makefile
	@[ -f $(IMAGE) ] || dd if=/dev/zero of=$(IMAGE) count=$$((80*18*2))
	@dd if=boot/boot.bin of=$(IMAGE) count=1 conv=notrunc
	@dd if=kernel/kernel.bin of=$(IMAGE) count=80 seek=1 conv=notrunc

boot/boot.bin: boot/boot.s
	nasm -f bin -o boot/boot.bin boot/boot.s

kernel/kernel.bin: $(wildcard kernel/*.s) $(wildcard kernel/*.c) \
                   kernel/kernel.lds kernel/Makefile
	(cd kernel; make)
