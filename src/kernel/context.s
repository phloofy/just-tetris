	.global context_switch
context_switch:
	mov 4(%esp),%eax	# dest task state
	mov 8(%esp),%edx	# src task state
	mov %esp,%ecx

	pushf
	
	push %ebx
	push %ebp
	push %esi
	push %edi

	mov %esp,0(%eax)	# store stack
	mov 0(%edx),%esp	# load stack

	push 4(%edx)
	push 12(%ecx)
	mov 16(%ecx),%eax
	call *%eax
	add $8,%esp

	pop %edi
	pop %esi
	pop %ebp
	pop %ebx

	popf

	ret
