	.global context_switch
context_switch:
	mov 4(%esp),%eax	# target task state
	mov 8(%esp),%edx	# source task state
	mov %esp,%ecx

	pushf
	
	push %ebx
	push %ebp
	push %esi
	push %edi

	cli
	
	mov %esp,0(%edx)	# store source stack
	mov 0(%eax),%esp	# load target stack

	push 4(%eax)		# target task
	push 12(%ecx)		# callback
	mov 16(%ecx),%eax	# invoke
	call *%eax
	add $8,%esp

	pop %edi
	pop %esi
	pop %ebp
	pop %ebx

	popf

	ret
