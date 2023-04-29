	.section .mbr16,"awx",@progbits
	.code16
	.global start
start:	
	cli			# disable interrupts

	xor %ax,%ax		# initialize data and stack segments
	mov %ax,%ds
	mov %ax,%ss
	
	movb %dl,0x7000		# save boot drive id	

	movw $30,0x6000		# read boot drive parameters from bios
	movb $0x48,%ah
	movb 0x7000,%dl
	movw $0x6000,%si
	int $0x13

	movw 0x6010,%cx		# sector count (low 16 bits)

	mov $0x7e0,%di		# next sector address
	mov $1,%bp		# next sector number

1:	
	movw $0x6000,%si	# DAP pointer
	movb $16,(%si)
	movb $0,1(%si)
	sub $1,%cx

	cmp $0,%cx
	jz 1f

	movw $1,2(%si)
	movw $0,4(%si)
	movw %di,6(%si)
	add $0x20,%di
	movw %bp,8(%si)
	inc %bp
	movw $0,10(%si)
	movw $0,12(%si)
	movw $0,14(%si)

	mov $0x42,%ah
	movb 0x7000,%dl
	int $0x13
	jmp 1b
1:
	xor %cx,%cx
	xor %bx,%bx
	xor %dx,%dx
	mov $0xE801,%ax
	int $0x15

	mov %ax,mem_info_ax
	mov %bx,mem_info_bx
	mov %cx,mem_info_cx
	mov %dx,mem_info_dx

	mov $0x8000,%ax
	jmp *%ax
	
end:
	.skip 510 - (end - start)
	.byte 0x55
	.byte 0xAA

	# 0x7e00
kernel_load_adress:

	# 0x8000
	.section .entry16,"awx",@progbits
	.global smp_entry
smp_entry:
	cli
	xor %ax,%ax
	mov %ax,%ds
	mov %ax,%ss

	lgdt gdt_desc
	lidt idt_desc

	mov %cr0,%eax
	or $1,%eax
	mov %eax,%cr0

	ljmp $8,$kernel_entry

	.global tss
tss:
	.skip 104 * 16

	.section .entry32,"awx",@progbits
	.code32
kernel_entry:
	mov $0x10,%eax
	mov %eax,%ds
	mov %eax,%es
	mov %eax,%fs
	mov %eax,%gs
	mov $0x18,%eax
	mov %eax,%ss
	mov $0x28,%eax

	.extern kernel_stack
	.extern kernel_init

	mov $tmp_stack,%esp
	call kernel_stack
	mov %eax,%esp
	call kernel_init
	ud2

	.global mem_info
mem_info:	
mem_info_ax:
	.word 0
mem_info_bx:
	.word 0
mem_info_cx:
	.word 0
mem_info_dx:
	.word 0

gdt:
	.long 0
	.long 0
	.long 0x0000FFFF 	# kernel CS
	.long 0x00CF9A00
	.long 0x0000FFFF	# DS
	.long 0x00CFF200
	.long 0x0000FFFF	# kernel SS
	.long 0x00CF9200
	.long 0x0000FFFF	# user CS
	.long 0x00CFFA00

	.word 104		# TSS
	.word tss + 0 * 104
	.long 0x00008900
	.word 104
	.word tss + 1 * 104
	.long 0x00008900
	.word 104
	.word tss + 2 * 104
	.long 0x00008900
	.word 104
	.word tss + 3 * 104
	.long 0x00008900
	.word 104
	.word tss + 4 * 104
	.long 0x00008900
	.word 104
	.word tss + 5 * 104
	.long 0x00008900
	.word 104
	.word tss + 6 * 104
	.long 0x00008900
	.word 104
	.word tss + 7 * 104
	.long 0x00008900
	.word 104
	.word tss + 8 * 104
	.long 0x00008900
	.word 104
	.word tss + 9 * 104
	.long 0x00008900
	.word 104
	.word tss + 10 * 104
	.long 0x00008900
	.word 104
	.word tss + 11 * 104
	.long 0x00008900
	.word 104
	.word tss + 12 * 104
	.long 0x00008900
	.word 104
	.word tss + 13 * 104
	.long 0x00008900
	.word 104
	.word tss + 14 * 104
	.long 0x00008900
	.word 104
	.word tss + 15 * 104
	.long 0x00008900
gdt_end:

gdt_desc:
	.word gdt_end - gdt - 1
	.long gdt

	.global kernel_cs
kernel_cs:
	.long 8

	.global kernel_ds
kernel_ds:
	.long 16

	.global kernel_ss
kernel_ss:
	.long 24

	.global user_cs
user_cs:
	.long 32 + 3

	.global user_ss
user_ss:
	.long 16 + 3

	.global tss_desc
tss_desc:
	.long 40

	.global idt
	.align 64
idt:
	.skip 256 * 8, 0
idt_end:

idt_desc:
	.word idt_end - idt - 1
	.long idt

	.data
	.align 16
	.skip 4096
tmp_stack:
	.word 0
