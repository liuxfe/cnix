QEMU    = /opt/qemu/qemu-system-x86_64 -L /opt/qemu/ \
          -monitor stdio -display sdl
IMAGE   = cnix-fda.img

run: $(IMAGE)
	$(QEMU) -fda $(IMAGE)

$(IMAGE): boot.bin Makefile
	[ -f $(IMAGE) ] || dd if=/dev/zero of=$(IMAGE) count=$$((80*18*2))
	dd if=boot.bin of=$(IMAGE) count=1 conv=notrunc

boot.bin: boot.s
	nasm -f bin -o boot.bin boot.s
