	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"test.ll"
	.def	"😎";
	.scl	2;
	.type	32;
	.endef
	.globl	"😎"                            # -- Begin function 😎
	.p2align	4, 0x90
"😎":                                   # @"\F0\9F\98\8E"
# %bb.0:
	movl	%ecx, %eax
	negb	%al
	retq
                                        # -- End function
	.def	"Nc::unary-_i16";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary-_i16"                # -- Begin function Nc::unary-_i16
	.p2align	4, 0x90
"Nc::unary-_i16":                       # @"Nc::unary-_i16"
# %bb.0:
	movl	%ecx, %eax
	negl	%eax
                                        # kill: def $ax killed $ax killed $eax
	retq
                                        # -- End function
	.def	"Nc::unary-_i32";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary-_i32"                # -- Begin function Nc::unary-_i32
	.p2align	4, 0x90
"Nc::unary-_i32":                       # @"Nc::unary-_i32"
# %bb.0:
	movl	%ecx, %eax
	negl	%eax
	retq
                                        # -- End function
	.def	"Nc::unary-_i64";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary-_i64"                # -- Begin function Nc::unary-_i64
	.p2align	4, 0x90
"Nc::unary-_i64":                       # @"Nc::unary-_i64"
# %bb.0:
	movq	%rcx, %rax
	negq	%rax
	retq
                                        # -- End function
	.def	"Nc::unary-_i128";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary-_i128"               # -- Begin function Nc::unary-_i128
	.p2align	4, 0x90
"Nc::unary-_i128":                      # @"Nc::unary-_i128"
# %bb.0:
	movq	%rcx, %rax
	xorl	%ecx, %ecx
	negq	%rax
	sbbq	%rdx, %rcx
	movq	%rcx, %rdx
	retq
                                        # -- End function
	.def	"Nc::unary~_i8";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary~_i8"                 # -- Begin function Nc::unary~_i8
	.p2align	4, 0x90
"Nc::unary~_i8":                        # @"Nc::unary~_i8"
# %bb.0:
	movl	%ecx, %eax
	notb	%al
	retq
                                        # -- End function
	.def	"Nc::unary~_i16";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary~_i16"                # -- Begin function Nc::unary~_i16
	.p2align	4, 0x90
"Nc::unary~_i16":                       # @"Nc::unary~_i16"
# %bb.0:
	movl	%ecx, %eax
	notl	%eax
                                        # kill: def $ax killed $ax killed $eax
	retq
                                        # -- End function
	.def	"Nc::unary~_i32";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary~_i32"                # -- Begin function Nc::unary~_i32
	.p2align	4, 0x90
"Nc::unary~_i32":                       # @"Nc::unary~_i32"
# %bb.0:
	movl	%ecx, %eax
	notl	%eax
	retq
                                        # -- End function
	.def	"Nc::unary~_i64";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary~_i64"                # -- Begin function Nc::unary~_i64
	.p2align	4, 0x90
"Nc::unary~_i64":                       # @"Nc::unary~_i64"
# %bb.0:
	movq	%rcx, %rax
	notq	%rax
	retq
                                        # -- End function
	.def	"Nc::unary~_i128";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::unary~_i128"               # -- Begin function Nc::unary~_i128
	.p2align	4, 0x90
"Nc::unary~_i128":                      # @"Nc::unary~_i128"
# %bb.0:
	movq	%rcx, %rax
	notq	%rax
	notq	%rdx
	retq
                                        # -- End function
	.def	"Nc::binary+_i8";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::binary+_i8"                # -- Begin function Nc::binary+_i8
	.p2align	4, 0x90
"Nc::binary+_i8":                       # @"Nc::binary+_i8"
# %bb.0:
                                        # kill: def $dl killed $dl def $rdx
                                        # kill: def $cl killed $cl def $rcx
	leal	(%rcx,%rdx), %eax
                                        # kill: def $al killed $al killed $eax
	retq
                                        # -- End function
	.def	"Nc::binary+_i16";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::binary+_i16"               # -- Begin function Nc::binary+_i16
	.p2align	4, 0x90
"Nc::binary+_i16":                      # @"Nc::binary+_i16"
# %bb.0:
                                        # kill: def $dx killed $dx def $rdx
                                        # kill: def $cx killed $cx def $rcx
	leal	(%rcx,%rdx), %eax
                                        # kill: def $ax killed $ax killed $eax
	retq
                                        # -- End function
	.def	"Nc::binary+_i32";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::binary+_i32"               # -- Begin function Nc::binary+_i32
	.p2align	4, 0x90
"Nc::binary+_i32":                      # @"Nc::binary+_i32"
# %bb.0:
                                        # kill: def $edx killed $edx def $rdx
                                        # kill: def $ecx killed $ecx def $rcx
	leal	(%rcx,%rdx), %eax
	retq
                                        # -- End function
	.def	"Nc::binary+_i64";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::binary+_i64"               # -- Begin function Nc::binary+_i64
	.p2align	4, 0x90
"Nc::binary+_i64":                      # @"Nc::binary+_i64"
# %bb.0:
	leaq	(%rcx,%rdx), %rax
	retq
                                        # -- End function
	.def	"Nc::binary+_i128";
	.scl	2;
	.type	32;
	.endef
	.globl	"Nc::binary+_i128"              # -- Begin function Nc::binary+_i128
	.p2align	4, 0x90
"Nc::binary+_i128":                     # @"Nc::binary+_i128"
# %bb.0:
	movq	%rcx, %rax
	addq	%r8, %rax
	adcq	%r9, %rdx
	retq
                                        # -- End function
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
	subq	$32, %rsp
	.seh_stackalloc 32
	leaq	32(%rsp), %rbp
	.seh_setframe %rbp, 32
	.seh_endprologue
	callq	__main
	movl	$232, %ecx
	callq	"Nc::unary~_i32"
	leaq	.L.str(%rip), %rcx
	movl	%eax, %edx
	callq	printf
	xorl	%eax, %eax
	addq	$32, %rsp
	popq	%rbp
	retq
	.seh_endproc
                                        # -- End function
	.section	.rdata,"dr"
.L.str:                                 # @.str
	.asciz	"i32[%d]\n"

