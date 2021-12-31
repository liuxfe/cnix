extern do_div_zero, do_debug, do_nmi, do_breakpoint, do_overflow
extern do_bound_range, do_invalid_opcode, do_device_not_invalid
extern do_double_fault, do_invalid_tss, do_segment_not_exsit
extern do_stack_segment_fault, do_general_protection, do_page_fault
extern do_x87_fpu_error, do_align_check, do_machine
extern do_SMID_fault, do_reserved_trap
extern do_default_ignore, do_clock, do_lvt_timer, do_ide0

global int_div_zero, int_debug, int_nmi, int_breakpoint, int_overflow
global int_bound_range, int_invalid_opcode, int_device_not_invalid
global int_double_fault, int_reserved_trap, int_invalid_tss
global int_segment_not_exsit, int_stack_segment_fault
global int_general_protection, int_page_fault,int_x87_fpu_error
global int_align_check, int_machine_check, int_SIMD_fault
global int_default_ignore, int_clock, int_lvt_timer, int_ide0
global ret_from_kernel_trap

	section .text
	bits 64
int_div_zero:
	push	r15
	push	r14
	lea	r14, [rel do_div_zero]
	jmp	int_noerr_comm

int_debug:
	push	r15
	push	r14
	lea	r14, [rel do_debug]
	jmp	int_noerr_comm

int_nmi:
	push	r15
	push	r14
	lea	r14, [rel do_nmi]
	jmp	int_noerr_comm

int_breakpoint:
	push	r15
	push	r14
	lea	r14, [rel do_breakpoint]
	jmp	int_noerr_comm

int_overflow:
	push	r15
	push	r14
	lea	r14, [rel do_overflow]
	jmp	int_noerr_comm

int_bound_range:
	push	r15
	push	r14
	lea	r14, [rel do_bound_range]
	jmp	int_noerr_comm

int_invalid_opcode:
	push	r15
	push	r14
	lea	r14, [rel do_invalid_opcode]
	jmp	int_noerr_comm

int_device_not_invalid:
	push	r15
	push	r14
	lea	r14, [rel do_device_not_invalid]
	jmp	int_noerr_comm

int_double_fault:
	push	r14
	lea	r14, [rel do_double_fault]
	jmp	int_noerr_comm

int_reserved_trap:
	push	r15
	push	r14
	lea	r14, [rel do_reserved_trap]
	jmp	int_noerr_comm

int_invalid_tss:
	push	r14
	lea	r14, [rel do_invalid_tss]
	jmp	int_noerr_comm

int_segment_not_exsit:
	push	r14
	lea	r14, [rel do_segment_not_exsit]
	jmp	int_noerr_comm

int_stack_segment_fault:
	push	r14
	lea	r14, [rel do_stack_segment_fault]
	jmp	int_noerr_comm

int_general_protection:
	push	r14
	lea	r14, [rel do_general_protection]
	jmp	int_noerr_comm

int_page_fault:
	push	r14
	lea	r14, [rel do_page_fault]
	jmp	int_noerr_comm

int_x87_fpu_error:
	push	r15
	push	r14
	lea	r14, [rel do_x87_fpu_error]
	jmp	int_noerr_comm

int_align_check:
	push	r14
	lea	r14, [rel do_align_check]
	jmp	int_noerr_comm

int_machine_check:
	push	r15
	push	r14
	lea	r14, [rel do_machine]
	jmp	int_noerr_comm

int_SIMD_fault:
	push	r15
	push	r14
	lea	r14, [rel do_SMID_fault]
	jmp	int_noerr_comm

int_error_comm:
	xchg	qword r15, [rsp+8]
int_noerr_comm:
	push	r13
	push	r12
	push	r11
	push	r10
	push	r9
	push	r8
	push	rbp
	push	rsi
	push	rdi
	push	rdx
	push	rcx
	push	rbx
	push	rax
	mov	rax, es
	push	rax
	mov	rax, ds
	push	rax

	mov	rax, 0x10
	mov	ds,  ax
	mov	es,  ax

	mov	rsi, r15
	mov	rdi, rsp

	call	r14
	jmp	ret_from_kernel_trap

int_default_ignore:
	push	r15
	lea	r15, [rel do_default_ignore]
	jmp	intr_comm

int_clock:
	push	r15
	lea	r15, [rel do_clock]
	jmp	intr_comm

int_lvt_timer:
	push	r15
	lea	r15, [rel do_lvt_timer]
	jmp	intr_comm

int_ide0:
	push	r15
	lea	r15, [rel do_ide0]
	jmp	intr_comm
intr_comm:
	push	r14
	push	r13
	push	r12
	push	r11
	push	r10
	push	r9
	push	r8
	push	rbp
	push	rsi
	push	rdi
	push	rdx
	push	rcx
	push	rbx
	push	rax
	mov	rax, es
	push	rax
	mov	rax, ds
	push	rax

	mov	rax, 0x10
	mov	ds,  ax
	mov	es,  ax

	mov	rdi, rsp
	call	r15
	jmp	ret_from_kernel_trap

ret_from_kernel_trap:
	pop	rax
	mov	ds, ax
	pop	rax
	mov	es, ax
	pop	rax
	pop	rbx
	pop	rcx
	pop	rdx
	pop	rdi
	pop	rsi
	pop	rbp
	pop	r8
	pop	r9
	pop	r10
	pop	r11
	pop	r12
	pop	r13
	pop	r14
	pop	r15
	iretq
