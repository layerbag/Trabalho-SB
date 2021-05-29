.section .rodata
.data

.text

.globl f1
f1:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp

  movq %rsi, %r10

  movl $20, 16(%r10)

  movl $5, %eax
  movl %eax, -28(%rbp)

  movl -28(%rbp), %eax
  addl %edi, %eax

  movl %eax, %edi

  movl %edi, %eax

  movl $2, %ecx
  cltd
  idivl %ecx
  movl %eax, %edi

  movl %edi, %eax

  imull $4, %eax
  movl %eax, -28(%rbp)

  movl -28(%rbp), %eax
  subl $2, %eax
  movl %eax, -28(%rbp)

  cmpl $0, -28(%rbp)
  je after0

  movl 16(%rsi), %eax
movl %eax, -32(%rbp)

  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)

  movl -32(%rbp), %edi

  call f2
  movl %eax, -32(%rbp)

  movq -8(%rbp), %rdi
  movq -16(%rbp), %rsi
  movq -24(%rbp), %rdx

  movl -32(%rbp), %eax

 leave
 ret
after0:
  movl -28(%rbp), %eax
  addl $10, %eax
  movl %eax, -28(%rbp)

  movl -28(%rbp), %eax

 leave
 ret
