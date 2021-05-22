.section .rodata
.data

.text

.globl f1
f1:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp


leave
ret

.globl f2
f2:
  pushq %rbp
  movq %rsp, %rbp
  subq $160, %rsp


  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)

  movl -28(%rbp), %edi
  leaq -152(%rbp), %r10
  movq %r10, %rsi

  call f1
  movl %eax, -32(%rbp)

  movq -8(%rbp), %rdi
  movq -16(%rbp), %rsi
  movq -24(%rbp), %rdx

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

leave
ret

