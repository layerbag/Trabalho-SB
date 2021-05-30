#include <stdio.h>
#include <stdlib.h>

typedef struct funcoes{
  int tam_pilha,qtd_var;
  int *variavel;
}funcao;

void atribuicoes(char*line, funcao f){
  int dest,var1,var2;
  char p1,p2,op,t_dest;
  int r = sscanf(line, "%ci%d = %ci%d %c %ci%d",&t_dest,&dest,&p1,&var1,&op,&p2,&var2);

  if(r == 3){             // se for uma atribuição simples do tipo vi1 = vi2
    if(p1 == 'c'){        // se for vi1 = constante

      printf("  movl $%d, %d(%%rbp)\n\n", var1, f.variavel[dest]);  

    }else if (p1 == 'v'){                // se for vi1 = variavel
      printf("  movl %d(%%rbp), %%r10d\n", f.variavel[var1]);
      printf("  movl %%r10d, %d(%%rbp)\n\n",f.variavel[dest]);

    }else if (p1 == 'p'){
        if(var1 == 1) printf("movl %%edi, %d(%%rbp)\n\n", f.variavel[dest]);
        if(var1 == 2) printf("movl %%esi, %d(%%rbp)\n\n", f.variavel[dest]);
        if(var1 == 3) printf("movl %%edx, %d(%%rbp)\n\n", f.variavel[dest]);
    }
  }
  
  else if(r > 3){        // se for uma operação mais complexa
    

    printf("  movl ");    
    if(p1 == 'c'){

      printf("$%d, %%eax\n", var1);      // movl $x, %r10d

    }else if(p1 == 'v'){

      printf("%d(%%rbp), %%eax\n", f.variavel[var1]);    // movl -x(%rbp), %r10d

    }else if(p1 == 'p'){
        if(var1 == 1) printf("%%edi, %%eax\n\n");
        if(var1 == 2) printf("%%esi, %%eax\n\n");
        if(var1 == 3) printf("%%edx, %%eax\n\n");
    }


    if(op == '+') printf("  addl ");    // addl
    if(op == '*') printf("  imull ");   // imull
    if(op == '-') printf("  subl ");    //subl

    if(op != '/'){  // caso não for divisão faz os seguintes passos:

      if(p2 == 'c'){            

        printf("$%d, %%eax\n", var2);    // opera x(constante) ao registrador contendo o primeiro elemento

      }else if(p2 == 'v'){

        printf("%d(%%rbp), %%eax\n", f.variavel[var2]);  // opera x(variavel) ao registrador contendo o primeiro elemento

      }else if(p2 == 'p'){

        if(var2 == 1) printf("%%edi, %%eax\n\n");
        if(var2 == 2) printf("%%esi, %%eax\n\n");
        if(var2 == 3) printf("%%edx, %%eax\n\n");
        
      }

    }
    else{ // se for divisão faz:
        if(p2 == 'c'){
            printf("  movl $%d, %%ecx\n", var2);
        }
        else if(p2 == 'v'){

            printf("  movl %d(%%rbp), %%ecx\n", f.variavel[var2]);  // move var2 para %ecx

        }else if(p2 == 'p'){

            if(var2 == 1) printf("  movl %%edi, %%ecx\n");
            if(var2 == 2) printf("  movl %%esi, %%ecx\n");
            if(var2 == 3) printf("  movl %%edx, %%ecx\n");

        }
        printf("  cltd\n  idivl %%ecx\n");  // idivl de %ecx, o resultado fica em %eax
    }
    if(t_dest == 'v'){
        printf("  movl %%eax, %d(%%rbp)\n\n", f.variavel[dest]);   // move o registrador contendo a operação para o endereço da variavel de destino
    }else{
        if(dest == 1) printf("  movl %%eax, %%edi\n\n");
        if(dest == 2) printf("  movl %%eax, %%esi\n\n");
        if(dest == 3) printf("  movl %%eax, %%edx\n\n");
    }
  }

}

void set_array(funcao f, char * line){
  int array, index,value;
  char p1;

  sscanf(line,"set %*ca%d index ci%d with %ci%d",&array,&index,&p1,&value);   // extrai o numero do array, o indice do array, o tipo(c = constante, v = variavel, p = parametro), e o valor
  index = index * 4;

  if(line[4] == 'v'){     // caso o array seja variavel
    printf("  leaq %d(%%rbp), %%r10\n", f.variavel[array]);

  }else{          // caso o array seja parametro

    if(array == 1) printf("  movq %%rdi, %%r10\n\n");
    if(array == 2) printf("  movq %%rsi, %%r10\n\n");
    if(array == 3) printf("  movq %%rdx, %%r10\n\n");

  }

  if(p1 == 'c') printf("  movl $%d, %d(%%r10)\n\n", value, index);        // se for constante move x para o endereço do array na posição index

  else if(p1 == 'v')printf("  movl %d(%%rbp), %%r8d\n  movl %%r8d, %d(%%r10)\n\n", f.variavel[value], index);      // se for variavel, move a a variavel para r8d e depois r8d para o array no index
  
  else if(p1 == 'p'){

    if(value == 1) printf("  movl %%edi, %d(%%r10)\n\n",index);   // se for o primeiro parametro passa %edi para o array
    if(value == 2) printf("  movl %%esi, %d(%%r10)\n\n",index);   // se for o segundo parametro passa %esi para o array
    if(value == 3) printf("  movl %%edx, %d(%%r10)\n\n",index);   // se for o terceiro parametro passa %edx para o array
  }
}

void get_array(funcao f, char* line){
  char * temp;
  int c = 0,index,var;
  sscanf(line, "get %*ca%*c index ci%d", &index);
  index *= 4;
  temp = strtok(line, " ");

  while(temp != NULL){
    if(temp[0] == 'p' && c == 0){         // caso seja um parametro
      
      if(temp[2] == '1') printf("  movl %d(%%rdi), %%eax\n", index);  //rdi
      if(temp[2] == '2') printf("  movl %d(%%rsi), %%eax\n", index);  //rsi
      if(temp[2] == '3') printf("  movl %d(%%rdx), %%eax\n", index);  //rdx
      c = 1;

    }

    else if(temp[0] == 'v' && c == 0){    // caso seja variavel
      
      var = temp[2] - '0';
      printf("  leaq %d(%%rbp), %%r10\n  movl %d(%%r10), %%eax\n",f.variavel[var], index);  // leaq
        c = 1;
    }

    else if(temp[0] ==  'v' && c == 1){

      var = temp[2] - '0';
      printf("movl %%eax, %d(%%rbp)\n\n", f.variavel[var]);     // endereço variavel de destino

    }

    else if(temp[0] ==  'p' && c == 1){
      
      if(temp[2] == '1') printf("movl %%eax, %%edi\n\n");  //rdi
      if(temp[2] == '2') printf("movl %%eax, %%esi\n\n");  //rsi
      if(temp[2] == '3') printf("movl %%eax, %%edx\n\n");  //rdx

    }

    temp = strtok(NULL, " ");
  }
  free(temp);
}

void declaracao_int (funcao *f1, char* line){
  int  i1;
  void * usr;

    sscanf(line,"var vi%d", &i1);     // pega o numero da variavel
  f1->qtd_var++;                         // inc a quantidade de variaveis da funcao
  f1->tam_pilha += 4;                    // soma +4 no tamanho da pilha
        
  if(f1->qtd_var == 1){
    f1->variavel = malloc(sizeof(int));  // se for o primeiro elemento faz o malloc
  }else{
    usr = realloc(f1->variavel,f1->qtd_var * sizeof(int));  // senão faz o realloc
  }

  f1->variavel[i1] = f1->tam_pilha * -1;    // posição do elemento adicionado na pilha

}

void declaracao_vet (funcao * f1, char* line){
  int i1,i2;
  void * usr;

  sscanf(line,"vet va%d size ci%d", &i1,&i2);
  f1->qtd_var++;
  f1->tam_pilha += i2*4;

  if(f1->qtd_var == 1){
    f1->variavel = malloc(sizeof(int));  // se for o primeiro elemento faz o malloc
  }else{
    usr = realloc(f1->variavel,f1->qtd_var * sizeof(int));  // senão faz o realloc
  }

  f1->variavel[i1] = f1->tam_pilha * -1;
  
}

void call_function (funcao*f, char *line){
  char *temp;
  char dest[3];
  char parametros[3][5];
  char *func = malloc(2);
  int param2[3] = {-8,-16,-24};
  int c = -1, index;

  strcpy(parametros[0], "%edi");    // preenche o vetor de strings com registradores de parametro
  strcpy(parametros[1], "%esi");
  strcpy(parametros[2], "%edx");

  temp = strtok(line, " ");
  strcpy(dest,temp);                // guarda o destino
  printf("  movq %%rdi, -8(%%rbp)\n  movq %%rsi, -16(%%rbp)\n  movq %%rdx, -24(%%rbp)\n\n");    // salva os registradores dos parametros

  while(temp != NULL){
    if(c != -1){        // ignorar primeira iteração do while

      if(temp[0] == 'f'){     // se for um f, guarda o nome da função
        strcpy(func, temp); 
      }

      else if(temp[0] == 'c' && temp[1] == 'i'){      // se for um "ci" é constante
        sscanf(temp, "ci%d", &index);
        printf("  movl $%d, %s\n", index, parametros[c]);   // move o valor da constante para o parametro[c];
        c++;

      }
      
      else if(temp[0] == 'v' && temp[1] == 'i'){      // se for "vi" é uma variavel do tipo inteiro

        sscanf(temp,"vi%d", &index);
        printf("  movl %d(%%rbp), %s\n",f->variavel[index], parametros[c]);     // move para o registrador de parametro[c]
        c++;

      }
      
      else if(temp[0] == 'v' && temp[1] == 'a'){        // se for um "va" é um array de inteiro

        sscanf(temp,"va%d", &index);
        printf("  leaq %d(%%rbp), %%r10\n", f->variavel[index]);    // faz o leaq para r10
        parametros[c][1] = 'r';
        printf("  movq %%r10, %s\n", parametros[c]);      // move r10 para o parametro desejado
        parametros[c][1] = 'e';
        c++;

      }
      
      else if(temp[0] == 'p' && temp[1] == 'i'){      // se for "pi" é um parametro do tipo inteiro
        
        index = temp[2] - '0';                  
        printf("  movl %d(%%rbp), %%r10d\n", param2[index]);       // pega o parametro salvo na pilha, e passa para o parametro atual
        printf("  movl %%r10d, %s\n", parametros[c]);
        c++;

      }
      
      else if(temp[0] == 'p' && temp[1] == 'a'){    // se for "pa" é um parametro que é array

        index = temp[2] - '0';
        printf("  leaq %d(%%rbp), %%r10\n", param2[index]);     // faz o leaq do parametro salvo na memoria
        parametros[c][1] = 'r'; 
        printf("  movq %%r10, %s\n", parametros[c]);      // move para o parametro atual
        parametros[c][1] = 'e';
        c++;

      }
    }

    if(c == -1) c = 0;        // atualiza c na primeira iteração
    temp = strtok(NULL, " ");
  }

  printf("\n  call %s\n", func);    // printa o chamdo da função
 
  if(dest[0] == 'v' && dest[1] == 'i'){       // move para a variavel de destino dependendo do seu tipo

    index = dest[2] - '0';
    printf("  movl %%eax, %d(%%rbp)\n\n",f->variavel[index]);

  }
  
  else if(dest[0] == 'v' && dest[1] == 'a'){

    index = dest[2] - '0';
    printf("  movq %%rax, %d(%%rbp)\n\n", f->variavel[index]);

  }
  
  else if(dest[0] == 'p' && dest[1] == 'i'){
        
    index = dest[2] - '0';
    printf("  movl %%eax, %d(%%rbp)\n\n", param2[index]);

  }
  
  else if(dest[1] == 'a'){

    index = dest[2] - '0';
    printf("  movq %%rax, %d(%%rbp)\n\n", param2[index]);
  
  }
  printf("  movq -8(%%rbp), %%rdi\n  movq -16(%%rbp), %%rsi\n  movq -24(%%rbp), %%rdx\n\n");

  free(func);
  free(temp);
}

void condicional(funcao f, char* line, int cont){
  int val;
  char tipo;

  sscanf(line,"if %ci%d", &tipo, &val);
  
  if(tipo == 'v'){
    printf("  cmpl $0, %d(%%rbp)\n", f.variavel[val]);
  }

  if(tipo == 'p'){
    
    if(val == 1) printf("  cmpl $0, %%edi\n");
    if(val == 2) printf("  cmpl $0, %%esi\n");
    if(val == 3) printf("  cmpl $0, %%edx\n");

  }

  if(tipo == 'c'){

    printf("  movl $%d, %%r10d\n", val);
    printf("  cmpl $0, %%r10d\n");

  }

  printf("  je after%d\n\n", cont);
}

void returns(char *line, funcao f){
    int i1;
    char p1;

    // retorna constante
    if(sscanf(line, "return ci%d",&i1) == 1){ 
      printf("  movl $%d, %%eax\n",i1);
    }
    
    // retorna parametro
    if(strncmp(line, "return p",8) == 0){   

      sscanf(line,"return p%c%d",&p1,&i1);
      if(p1 == 'i'){      // se for int

        if(i1 == 1) {                 
          printf("  movl %%edi, %%eax\n\nleave\nret\n");            // se for o primeiro parametro
        }
        if(i1 == 2) {
          printf("  movl %%esi, %%eax\n\nleave\nret\n");            // se for o segundo parametro
        }
        if(i1 == 3) {
          printf("  movl %%edx, %%eax\n\nleave\nret\n");            // se for o terceiro parametro
        }

      }else{    // se for array

        if(i1 == 1) {                 
          printf("  movq %%rdi, %%rax\n\nleave\nret\n");            // se for o primeiro parametro
        }
        if(i1 == 2) {
          printf("  movq %%rsi, %%rax\n\nleave\nret\n");            // se for o segundo parametro
        }
        if(i1 == 3) {
          printf("  movq %%rdx, %%rax\n\nleave\nret\n");            // se for o terceiro parametro
        }
      }
    }

    // retorna variavel
    if(strncmp(line, "return v", 8) == 0){

      sscanf(line, "return v%*c%d", &i1);

      if(line[8] == 'i'){
        
        printf("  movl %d(%%rbp), %%eax\n\n leave\n ret\n", f.variavel[i1]);
        return;

      }else{

        printf("  leaq %d(%%rbp), %%rax\nleave\nret\n", f.variavel[i1]);

      }
    }  


}