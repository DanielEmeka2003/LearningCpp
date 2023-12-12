	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test.ll"
	.def	main;
	.scl	2;
	.type	32;
	.endef
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
main:                                   # @main
.seh_proc main
# %bb.0:                                # %l_entry
	pushq	%rbp
	.seh_pushreg %rbp
	pushq	%rsi
	.seh_pushreg %rsi
	subq	$40, %rsp
	.seh_stackalloc 40
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	xorl	%esi, %esi
	callq	__main
	testb	%sil, %sil
	jne	.LBB0_2
# %bb.1:                                # %l_andClause2
	movl	$1, %esi
.LBB0_2:                                # %l_postAnd
	movl	%esi, %eax
	addq	$40, %rsp
	popq	%rsi
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
