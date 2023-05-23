	.global inb
inb:
	mov 4(%esp),%dx
	inb %dx,%al
	ret

	.global inw
inw:
	mov 4(%esp),%dx
	inw %dx,%ax
	ret

	.global inl
inl:
	mov 4(%esp),%dx
	inl %dx,%eax
	ret

	.global outb
outb:
	mov 4(%esp),%dx
	mov 8(%esp),%al
	outb %al,%dx
	ret

	.global outw
outw:
	mov 4(%esp),%dx
	mov 8(%esp),%ax
	outw %ax,%dx
	ret

	.global outl
outl:
	mov 4(%esp),%dx
	mov 8(%esp),%eax
	outl %eax,%dx
	ret

	.global monitor
monitor:
	mov 4(%esp),%eax
	xor %ecx,%ecx
	xor %edx,%edx
	monitor
	ret

	.global mwait
mwait:
	mov %eax,%eax
	xor %ecx,%ecx
	xor %edx,%edx
	mwait
	ret

	.global pause
pause:
	pause
	ret

	.global rdmsr
rdmsr:
	mov 4(%esp),%ecx
	rdmsr
	ret

	.global wrmsr
wrmsr:
	mov 4(%esp),%ecx
	mov 8(%esp),%eax
	mov 12(%esp),%edx
	wrmsr
	ret
	
	/* interrupts */
	
	.global get_flags
get_flags:
	pushf
	pop %eax
	ret

	.global int_disable
int_disable:
	pushf
	pop %eax
	and $0x200,%eax
	jz 1f
	cli
1:
	ret

	.global int_restore
int_restore:
	mov 4(%esp),%eax
	test %eax,%eax
	jz 1f
	sti
1:
	ret
	
	.global cpuid
cpuid:
	push %ebx
	push %esi
	mov 12(%esp),%eax
	cpuid
	mov 16(%esp),%esi
	mov %eax,0(%esi)
	mov %ebx,4(%esi)
	mov %ecx,8(%esi)
	mov %edx,12(%esi)
	pop %esi
	pop %ebx
	ret
