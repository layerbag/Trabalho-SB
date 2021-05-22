.section .rodata
.data

.text

.globl f2
f2:
  pushq %rbp
  movq %rsp, %rbp
  subq $160, %rsp

  movl $5, -28(%rbp)

  cmpl $0, -28(%rbp)
  jnz after0

  movl -28(%rbp), %r10d
  imull -32(%rbp), %r10d
  movl %r10d, -28(%rbp)

  movl $1, %r10d
  subl $3, %r10d
  movl %r10d, -28(%rbp)

after0:
  movl $1, -32(%rbp)

  leaq -152(%rbp), %r10
  movl $2, 5(%r10)

  movl 20(%rdi), -28(%rbp)

  cmpl $0, -28(%rbp)
  jnz after1

  movl -28(%rbp), %eax

 leave
 ret
after1:
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

  movl -32(%rbp), %eax

 leave
 ret
