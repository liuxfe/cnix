PHYOFF	EQU	0xFFFF800000000000
KSTART	EQU	0x1000

	bits	16
startup16:
	cli
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov 	sp, 0x1000

	; get memory map
        mov	edi, 0x500;e820map - KOFFSET
        xor	ebx, ebx
    .next:
        mov	eax, 0xE820
        mov	ecx, 20
        mov	edx, 0x534D4150
        int	15h
        jc	.faild
        cmp	ebx, 0
        je	.done
        add	edi, 20
        jmp	.next
    .faild:
        jmp	$
    .done:
	mov	dword es:[edi+16], 0

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

	lgdt	[rel gdt_ptr]

	lidt	[rel idt_ptr]

	call	cstartup
	jmp	mpstartup

gdt_ptr:
	dw 4096 - 1
	dq GDTtab
idt_ptr:
	dw 256 * 8 * 2 - 1
	dq IDTtab

	bits 64
trap_div_zero:
	push r15
	push r14
	lea  r14, [rel do_div_zero]
	jmp  trap_noerr_comm

trap_debug:
	push r15
	push r14
	lea  r14, [rel do_debug]
	jmp  trap_noerr_comm

trap_nmi:
	push r15
	push r14
	lea  r14, [rel do_nmi]
	jmp  trap_noerr_comm

trap_breakpoint:
	push r15
	push r14
	lea  r14, [rel do_breakpoint]
	jmp  trap_noerr_comm

trap_overflow:
	push r15
	push r14
	lea  r14, [rel do_overflow]
	jmp  trap_noerr_comm

trap_bound_range:
	push r15
	push r14
	lea  r14, [rel do_bound_range]
	jmp  trap_noerr_comm

trap_invalid_opcode:
	push r15
	push r14
	lea  r14, [rel do_invalid_opcode]
	jmp  trap_noerr_comm

trap_device_not_invalid:
	push r15
	push r14
	lea  r14, [rel do_device_not_invalid]
	jmp  trap_noerr_comm

trap_double_fault:
	push r14
	lea  r14, [rel do_double_fault]
	jmp  trap_noerr_comm

trap_reserved:
	push r15
	push r14
	lea  r14, [rel do_reserved_trap]
	jmp  trap_noerr_comm

trap_invalid_tss:
	push r14
	lea  r14, [rel do_invalid_tss]
	jmp  trap_noerr_comm

trap_segment_not_exsit:
	push r14
	lea  r14, [rel do_segment_not_exsit]
	jmp  trap_noerr_comm

trap_stack_segment_fault:
	push r14
	lea  r14, [rel do_stack_segment_fault]
	jmp  trap_noerr_comm

trap_general_protection:
	push r14
	lea  r14, [rel do_general_protection]
	jmp  trap_noerr_comm

trap_page_fault:
	push r14
	lea  r14, [rel do_page_fault]
	jmp  trap_noerr_comm

trap_x87_fpu_error:
	push r15
	push r14
	lea  r14, [rel do_x87_fpu_error]
	jmp  trap_noerr_comm

trap_align_check:
	push r14
	lea  r14, [rel do_align_check]
	jmp  trap_noerr_comm

trap_machine_check:
	push r15
	push r14
	lea  r14, [rel do_machine]
	jmp  trap_noerr_comm

trap_SIMD_fault:
	push r15
	push r14
	lea  r14, [rel do_SMID_fault]
	jmp  trap_noerr_comm

trap_error_comm:
	xchg qword r15, [rsp+8]
trap_noerr_comm:
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rsi
	push rdi
	push rdx
	push rcx
	push rbx
	push rax
	mov  rax, es
	push rax
	mov  rax, ds
	push rax

	mov  rax, 0x10
	mov  ds,  ax
	mov  es,  ax

	mov  rsi, r15
	mov  rdi, rsp

	call r14

	jmp ret_from_kernel_trap

trap_ignore_intr:
	push r15
	lea  r15, [rel do_ignore_intr]
	jmp  intr_comm

intr_comm:
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rsi
	push rdi
	push rdx
	push rcx
	push rbx
	push rax
	mov  rax, es
	push rax
	mov  rax, ds
	push rax

	mov  rax, 0x10
	mov  ds,  ax
	mov  es,  ax

	mov  rdi, rsp
	call r15
	jmp  ret_from_kernel_trap

ret_from_kernel_trap:
	pop  rax
	mov  ds, ax
	pop  rax
	mov  es, ax
	pop  rax
	pop  rbx
	pop  rcx
	pop  rdx
	pop  rdi
	pop  rsi
	pop  rbp
	pop  r8
	pop  r9
	pop  r10
	pop  r11
	pop  r12
	pop  r13
	pop  r14
	pop  r15
	iretq

	section .data
GDTtab:
	dq 0
	dq 0x0020980000000000   ; Kernel Code
	dq 0x0000920000000000   ; Kernel Data
	dq 0x0020f80000000000   ; User Code
	dq 0x0000f20000000000   ; User Data
	times 4096 - 5 * 8 db 0	; Tss
IDTtab:
	times 256 * 2	dq 0

	global PML4E, PDPE0, PDT0, PTE0,IDTtab, GDTtab
	section .bss
PML4E:	resb	4096
PDPE0:	resb	4096
PDT0:	resb	4096
PTE0:	resb	4096


extern do_div_zero, do_debug, do_nmi, do_breakpoint, do_overflow
extern do_bound_range, do_invalid_opcode, do_device_not_invalid
extern do_double_fault, do_invalid_tss, do_segment_not_exsit
extern do_stack_segment_fault, do_general_protection, do_page_fault
extern do_x87_fpu_error, do_align_check, do_machine
extern do_SMID_fault, do_reserved_trap

global trap_div_zero, trap_debug, trap_nmi, trap_breakpoint, trap_overflow
global trap_bound_range, trap_invalid_opcode, trap_device_not_invalid
global trap_double_fault, trap_reserved, trap_invalid_tss
global trap_segment_not_exsit, trap_stack_segment_fault
global trap_general_protection, trap_page_fault,trap_x87_fpu_error
global trap_align_check, trap_machine_check, trap_SIMD_fault

extern do_ignore_intr
global trap_ignore_intr

extern cstartup, mpstartup
global startup16, startup32, startup64
