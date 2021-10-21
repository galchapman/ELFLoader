	global _start
section .text
_start:
	mov rax, 1
	mov rdi, rax
	mov rsi, message
	mov rdx, 14
	syscall
	mov rax, 60
	mov rdi, 0
	syscall
section .data
	message db "Hello, World", 0Ah
