global _start
section .data
section .bss
	tape resb 3000
section .text
_start:
	lea rbx,[tape]
	inc byte [rbx]
	inc byte [rbx]
loop_start0:
	cmp byte [rbx],0
	je loop_end1
	inc byte [rbx]
	cmp byte [rbx],0
	loop_start0
loop_end1:
	mov rax, 60
	xor rdi,rdi
	syscall