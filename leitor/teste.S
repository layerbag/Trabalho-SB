.section .rodata
.data

.text

.globl f1
f1:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp

  movl %edi, %eax

leave
ret
.globl f2
f2:
  pushq %rbp
  movq %rsp, %rbp
  subq $160, %rsp

  movl -32(%rbp), %r10d
  movl %r10d, -28(%rbp)

  movl -28(%rbp), %eax
  imull -32(%rbp), %eax
  movl %eax, -28(%rbp)

  movl $1, %eax
  subl $3, %eax
  movl %eax, -28(%rbp)

  movl -28(%rbp), %eax
  movl -32(%rbp), %ecx
  cltd
  idivl %ecx
  movl %eax, -32(%rbp)

  leaq -152(%rbp), %r10
  movl $2, 20(%r10)

  movl 20(%rdi), -28(%rbp)

  cmpl $0, -28(%rbp)
  je after0

  movl $0, -28(%rbp)

after0:
  cmpl $0, -28(%rbp)
  je after1

  movl -28(%rbp), %eax

 leave
 ret
after1:
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)

  movl $5, %edi
  leaq -16(%rbp), %r10
  movq %r10, %rsi

  call f1
  movl %eax, -32(%rbp)

  movq -8(%rbp), %rdi
  movq -16(%rbp), %rsi
  movq -24(%rbp), %rdx

  movl -32(%rbp), %eax

 leave
 ret
