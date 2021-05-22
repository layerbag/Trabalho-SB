int array,index,var;
  char p1,p2;
  int r = sscanf(line, "get %ca%d index ci%d to %ci%d",&p1, &array, &index,&p2, &var); // extrai o tipo (p = parametro, v = variavel) a posição do array, o indice, o tipo da variavel e a posição da variavel

  if(p1 == 'v'){
    printf("  leaq %d(%%rbp), %%r10\n", f.variavel[array]);
    printf("  movl %d(%%r10), %%r8d\n", f.variavel[index]);
    printf("  movl %%r8d, %d(%%rbp)\n\n", f.variavel[var]);
  }else if(array == 1){
    printf("  movl %d(%%rdi), %%r8d\n", f.variavel[index]);
    printf("  movl %%r8d, %d(%%rbp)\n\n", f.variavel[var]);
  }else if(array == 2){
    printf("  movl %d(%%rsi), %%r8d\n", f.variavel[index]);
    printf("  movl %%r8d, %d(%%rbp)\n\n", f.variavel[var]);
  }else if(array == 3){
    printf("  movl %d(%%rdx), %%r8d\n", f.variavel[index]);
    printf("  movl %%r8d, %d(%%rbp)\n\n", f.variavel[var]);
  }