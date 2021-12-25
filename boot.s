	bits	16

	org	0x0500
_start:
	xor	ax, ax
	mov	ds, ax
	mov	ss, ax
	mov	sp, 0x1000

	mov	cx, 256
	mov	si, 0x7C00
	mov	di, 0x0500
	rep movsw
	jmp	0:_go
_go:
	mov	si, msg
	call	disp_str

	jmp	$

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

msg:	db	"Hello World, CNIX !", 10, 13, 0

times	510 - ( $ - $$ ) db 0
	dw	0xAA55
