	bits	16

_start:
	jmp	startup16

startup16:
	cli
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	si, msg
	call	disp_str
	jmp	$

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
