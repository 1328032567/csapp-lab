4019a2  movq %rax, %rdi
4019c5  movq %rax, %rdi
401a06  movq %rsp, %rax

4019ab  popq %rax
4019cc  popq %rax

4019a3  movl %eax, %edi
4019c6  movl %eax, %edi

401a07  movl %esp, %eax

401a13  movl %ecx, %esi
401a27  movl %ecx, %esi

401a34  movl %edx, %ecx

401a3c  movl %esp, %eax

401a42  movl %eax, %edx

401a69  movl %edx, %ecx

401a86  movl %esp, %eax
401aae  movl %esp, %eax

4019dd  pop %rsp + movl %eax, %edx  5c 89 c2 90 c3

00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	ret 