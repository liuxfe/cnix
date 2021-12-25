PHYOFF	EQU	0xFFFF800000000000
KSTART	EQU	0x1000

	bits	16
	global	_start
_start:
	jmp	(startup16 - $$)

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
	jmp	dword 0x18:(startup32 - PHYOFF)	; goto 32 bits/protcted mode

_tmp_gdt:
	dq	0
	dq	0x0020980000000000	; Kernel 64bit Code
	dq	0x0000920000000000	; Kernel 64bit Data
	dq	0x00cf9a000000ffff	; Kernel 32bit Code
	dq	0x00cf92000000ffff	; Kernel 32bit Data
_tmp_gdtptr:
	dw	$ - _tmp_gdt - 1
	dd	_tmp_gdt - $$ + KSTART

	bits	32
startup32:
	cli
	mov	ax,  cs
	add	ax,  8
	mov	ds,  ax
	mov	es,  ax
	mov	ss,  ax
	mov	esp, 0x1000

	; tmp paging table at 0x100000
	mov	dword [0x100000], 0x101007      ; PML4E
	mov	dword [0x100800], 0x101007      ; PML4E
	mov	dword [0x101000], 0x102007      ; PDPE(PDPT0)
	mov	dword [0x102000], 0x000087      ; PDT (2M page)
	mov	dword [0x102008], 0x200087      ; PDT (2M page)

	; set pdtr
	mov	eax, 0x100000
	mov	cr3, eax

	; enbale PAE
	mov	eax, cr4
	bts	eax, 5
	mov	cr4, eax

	; enable long mode
	mov	ecx, 0x0C0000080  ; IA32_MSR_EFER
	rdmsr
	bts	eax, 8            ; EFER.LME
	wrmsr

	;enable paging
	mov	eax, cr0
	bts	eax, 31
	mov	cr0, eax

	lgdt	[_tmp_gdtptr - $$ + KSTART]
	jmp	dword 0x08:(startup64 -PHYOFF)

	bits	64
	extern 	cstartup
startup64:
	mov	rax, .go
	jmp     rax
.go:	cli
	mov	ax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax
	mov	rsp, 0x1000 + PHYOFF

	mov	dword [0x100000], 0      ; PML4E

	jmp	cstartup
	jmp	$
