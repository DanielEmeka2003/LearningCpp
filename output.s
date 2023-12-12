	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test.c"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %entry
	pushq	%rbp
	.seh_pushreg %rbp
	subq	$32, %rsp
	.seh_stackalloc 32
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	callq	__main
	movb	$1, %al
	testb	%al, %al
	jne	.LBB0_2
# %bb.1:                                # %true_case
	movl	$-1024, %eax                    # imm = 0xFC00
	jmp	.LBB0_3
.LBB0_2:                                # %false_case
	movl	$1000, %eax                     # imm = 0x3E8
.LBB0_3:                                # %post_cond
	addq	$32, %rsp
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
