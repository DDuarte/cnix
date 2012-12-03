.intel_syntax noprefix
.text
.globl __vg_swap_buffer

__vg_swap_buffer:
	push    ebp
	mov     ebp, esp
	sub     esp, 16

    push    edi
    push    esi

    mov     esi, DWORD PTR[ebp+8]

    mov     edi, DWORD PTR[ebp+12]

    mov     ecx, DWORD PTR[ebp+16]
    shr     ecx, 6

itr:
	jecxz cend

    movq    mm0,    [esi]
    movq    mm1,    [8+esi]
    movq    mm2,    [16+esi]
    movq    mm3,    [24+esi]
    movq    mm4,    [32+esi]
    movq    mm5,    [40+esi]
    movq    mm6,    [48+esi]
    movq    mm7,    [56+esi]

    movntq  [edi],      mm0
    movntq  [8+edi],    mm1
	movntq  [16+edi],   mm2
    movntq  [24+edi],   mm3
	movntq  [32+edi],   mm4
    movntq  [40+edi],   mm5
	movntq  [48+edi],   mm6
    movntq  [56+edi],   mm7

    add     edi,    64
    add     esi,    64
    dec     ecx

    jmp     itr

cend:

    pop     esi
    pop     edi

	leave
	ret

