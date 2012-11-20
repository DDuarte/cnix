.intel_syntax noprefix
.text
.globl __vg_swap_buffer

__vg_swap_buffer:
	push ebp
	mov ebp, esp
	sub esp, 16

	push eax

	xor eax, eax

itr:
	cmp eax, DWORD PTR[ebp+16]
	je end

	mov edx, eax
	add edx, [ebp+8]
	mov ebx, eax
	add ebx, [ebp+12]
	movzx edx, BYTE PTR [edx]
	mov BYTE PTR[ebx], dl
	inc eax
	jmp itr

end:

	pop eax
	leave
	ret

