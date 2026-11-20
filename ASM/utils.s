section .text

%macro init_function 1
	global	%1
	%1:
	push	ebp
	mov		ebp, esp
%endmacro

init_function strlen
	mov		eax, edi
	
	.loop:
		cmp		byte [eax], 0x0
		je		.end
		inc		eax
		jmp		.loop

	.end:
		sub		eax, edi
		pop		ebp
		ret

init_function strcmp
	.loop:
		movzx	eax, byte [edi]
		cmp		ah, byte [esi]
		jne		.end
		cmp		ah, 0x0
		je		.end
		jmp		.loop

	.end:
		sub		ah, byte [esi]
		pop		ebp
		ret

init_function atoi
