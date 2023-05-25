	.global spurious_handler_
spurious_handler_:
	iret

	.global timer_handler_
timer_handler_:
	pusha
	.extern timer_handler
	call timer_handler
	popa
	iret
