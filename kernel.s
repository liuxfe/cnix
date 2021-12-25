	bits	16
	org 0x1000

_start:
	jmp	startup16

startup16:
	cli
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov 	sp, 0x1000

	; enable A20
	in	al, 0x92
	or	al, 2
	out	0x92, al

	mov	eax, cr0
	bts	eax, 0
	mov	cr0, eax

	lgdt	cs:[_tmp_gdtptr - $$]
	jmp	dword 0x08:startup32 ; goto 32 bits code / protcted mode

_tmp_gdt:
	dq	0
	dq	0x00cf9a000000ffff  ; Kernel 32bit Code
	dq	0x00cf92000000ffff  ; Kernel 32bit Data
_tmp_gdtptr:
	dw	$ - _tmp_gdt - 1
	dd	_tmp_gdt

msg:	db	"Hello World, CNIX !", 13, 10, 0


; display a string
disp_str:
	push	ax
	push	bx
	mov	ah, 0x0e
	mov	bx, 0x10
    .next:
	mov	al, [si]
	cmp	al, 0
	je	.done
	int	0x10
	inc	si
	jmp	.next
    .done:
	pop	bx
	pop	ax
	ret

	bits	32
startup32:
	cli
	mov	ax,  cs
	add	ax,  8
	mov	ds,  ax
	mov	es,  ax
	mov	ss,  ax
	mov	esp, 0x1000

	mov ebp, 0xb8000
	mov dword [ebp], 0x2700 + 'C'
	mov dword [ebp+2], 0x2700 + 'N'
	mov dword [ebp+4], 0x2700 + 'I'
	mov dword [ebp+6], 0x2700 + 'X'
	mov dword [ebp+8], 0x2700 + '!'
	jmp $
