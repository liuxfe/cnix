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

load_kernel:
	mov	ax, 0x100
	mov	es, ax      ; buf es:bx
	xor	dl, dl      ; DH--head,DL--driver
    .next:
	xor	bx, bx
	mov	ch, [cur_cyl]
	mov	dh, [cur_head]
	mov	cl, [cur_sector]
	mov	ax, 0x0201
	int	0x13
	jc	.fail
	inc	byte [cur_sector]
	cmp	byte [cur_sector], 18+1
	jne	.no_head_inc
	mov	byte [cur_sector], 1
	xor	byte [cur_head], 0x1
	cmp	byte [cur_head], 0x0
	jne	.no_cyl_inc
	inc	byte [cur_cyl]
	mov	si, dot
	call	disp_str
    .no_cyl_inc:
    .no_head_inc:
	dec	byte [left_sectors]
	cmp	byte [left_sectors], 0
	je	.done
	mov	ax, es
	add	ax, 0x20
	mov	es, ax
	jmp	.next
    .fail:
	jmp	$
    .done:
	mov	si, ln
	call	disp_str

kill_motor:
	mov	dx, 0x3f2
	mov	al, 0
	out	dx, al

	jmp	0x100:0	; goto kernel

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

cur_cyl:	db 0
cur_head:	db 0
cur_sector:	db 2
left_sectors:	db 80
msg:		db "Loading",0
dot:		db ".",0
ln:		db 10, 13, 0  	;"\r\n\0"

times	510 - ( $ - $$ ) db 0
	dw	0xAA55
