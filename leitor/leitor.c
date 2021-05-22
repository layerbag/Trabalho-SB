#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 256


typedef struct funcoes{
  int tam_pilha,qtd_var;
  char str[LINESZ];
  int *variavel;
}funcao;

// Remove o '\n' do fim da linha
void remove_newline(char *ptr)
{
  while (*ptr) {
    
    if (*ptr == '\n')
      *ptr = 0;
    else
      ptr++;
  }
  
}

// -----------------------OPERAÇÕES-----------------------------------------------------------------
  
void operacoes(char*line, funcao f){
  int dest,var1,var2;
  char p1,p2,op;
  int r = sscanf(line, "vi%d = %ci%d %c %ci%d",&dest,&p1,&var1,&op,&p2,&var2);

  if(r == 3){             // se for uma atribuição simples do tipo vi1 = vi2
    if(p1 == 'c'){        // se for vi1 = constante

      printf("  movl $%d, %d(%%rbp)\n\n", var1, f.variavel[dest]);  

    }else{                // se for vi1 = variavel
      printf("  movl %d(%%rbp), %%r10d\n", f.variavel[var1]);
      printf("  movl %%r10d, %d(%%rbp)\n\n",f.variavel[dest]);

    }
  }
  
  else if(r > 3){        // se for uma operação mais complexa

    printf("  movl ");    
    if(p1 == 'c'){

      printf("$%d, %%r10d\n", var1);      // movl $x, %r10d

    }else{

      printf("%d(%%rbp), %%r10d\n", f.variavel[var1]);    // movl -x(%rbp), %r10d

    }


    if(op == '+') printf("  addl ");    // addl
    if(op == '*') printf("  imull ");   // imull
    if(op == '-') printf("  subl ");    //subl

    if(p2 == 'c'){            

      printf("$%d, %%r10d\n", var2);    // opera x(constante) ao registrador contendo o primeiro elemento

    }else{

      printf("%d(%%rbp), %%r10d\n", f.variavel[var2]);  // opera x(variavel) ao registrador contendo o primeiro elemento

    }

    printf("  movl %%r10d, %d(%%rbp)\n\n", f.variavel[dest]);   // move o registrador contendo a operação para o endereço da variavel de destino
  }

}

//-------------------------SET ARRAY-----------------------------------------------------------

void set_array(funcao f, char * line){
  int array, index,value;
  char p1;

  int r = sscanf(line,"set va%d index ci%d with %ci%d",&array,&index,&p1,&value);   // extrai o numero do array, o indice do array, o tipo(c = constante, v = variavel, p = parametro), e o valor
  printf("  leaq %d(%%rbp), %%r10\n", f.variavel[array]);

  if(p1 == 'c') printf("  movl $%d, %d(%%r10)\n\n", value, index);        // se for constante move x para o endereço do array na posição index

  else if(p1 == 'v')printf("  movl %d(%%rbp), %%r8d\n  movl %%r8d, %d(%%r10)\n\n", f.variavel[value], index);      // se for variavel, move a a variavel para r8d e depois r8d para o array no index
  
  else if(p1 == 'p'){

    if(value == 1) printf("  movl %%edi, %d(%%r10)\n\n",index);   // se for o primeiro parametro passa %edi para o array
    if(value == 1) printf("  movl %%esi, %d(%%r10)\n\n",index);   // se for o segundo parametro passa %esi para o array
    if(value == 1) printf("  movl %%edx, %d(%%r10)\n\n",index);   // se for o terceiro parametro passa %edx para o array

  }
}

//------------------------------------GET ARRAY-----------------------------------------

void get_array(funcao f, char* line){
  char * temp;
  int c = 0,index,var;
  sscanf(line, "get %*ca%*c index ci%d", &index);
  index *= 4;
  temp = strtok(line, " ");

  while(temp != NULL){
    if(temp[0] == 'p' && c == 0){         // caso seja um parametro
      
      if(temp[2] == '1') printf("  movl %d(%%rdi), ", index);  //rdi
      if(temp[2] == '2') printf("  movl %d(%%rsi), ", index);  //rsi
      if(temp[2] == '3') printf("  movl %d(%%rdx), ", index);  //rdx
      c = 1;

    }

    else if(temp[0] == 'v' && c == 0){    // caso seja variavel
      
      var = temp[2] - '0';
      printf("  leaq %d(%%rbp), %%r10\n  movl %d(%%r10), ",f.variavel[var], index);  // leaq

    }

    else if(temp[0] ==  'v' && c == 1){

      var = temp[2] - '0';
      printf("%d(%%rbp)\n\n", f.variavel[var]);     // endereço variavel de destino

    }

    else if(temp[0] ==  'p' && c == 1){
      
      if(temp[2] == '1') printf("%%edi\n\n");  //rdi
      if(temp[2] == '2') printf("%%esi\n\n");  //rsi
      if(temp[2] == '3') printf("%%edx\n\n");  //rdx

    }

    temp = strtok(NULL, " ");
  }
}

// ------------------------- CABEÇALHO DA FUNÇÃO------------------------------------
void cabecalho_funcao (char f, funcao* func){
  printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f); 

  func->tam_pilha = 24;       // seta tamanho da pilha inicial igual a 3*8 que são os parametros
  func->qtd_var = 0;        
}

void declaracao_int (funcao *f1, char* line){
  int r, i1;
  void * usr;

  r = sscanf(line,"var vi%d", &i1);     // pega o numero da variavel
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
  int r, i1,i2;
  void * usr;

  r = sscanf(line,"vet va%d size ci%d", &i1,&i2);
  f1->qtd_var++;
  f1->tam_pilha += i2*4;

  if(f1->qtd_var == 1){
    f1->variavel = malloc(sizeof(int));  // se for o primeiro elemento faz o malloc
  }else{
    usr = realloc(f1->variavel,f1->qtd_var * sizeof(int));  // senão faz o realloc
  }

  f1->variavel[i1] = f1->tam_pilha * -1;
  
}

int enddef(funcao* f1){     // comandos após as definições de variáveis
  int temp;


  if(f1->tam_pilha % 16 != 0){    // se o tamanho da pilha não for múltiplo de 16

    temp = f1->tam_pilha % 16;    // temp = o resto da divisão do tamanho da pilha por 16
    f1->tam_pilha += temp;        // e adiciona esse resto ao tamanho da pilha, a pilha agora é multiplo de 16

  }

  printf("  subq $%d, %%rsp\n\n", f1->tam_pilha); // printa a alocação da pilha

  return 0;
}

void call_function (funcao*f, char *line){
  char *temp;
  char dest[3];
  char parametros[3][5];
  char *func = malloc(2);
  int param2[3] = {-8,-16,-24};
  int c = -1, index, cont_p = 0;

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

        printf("  movl $%d, %s\n", temp[2]-'0', parametros[c]);   // move o valor da constante para o parametro[c];
        c++;

      }
      
      else if(temp[0] == 'v' && temp[1] == 'i'){      // se for "vi" é uma variavel do tipo inteiro

        index = temp[2] - '0';
        printf("  movl %d(%%rbp), %s\n",f->variavel[index], parametros[c]);     // move para o registrador de parametro[c]
        c++;

      }
      
      else if(temp[0] == 'v' && temp[1] == 'a'){        // se for um "va" é um array de inteiro

        index = temp[2] - '0';
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
}

//--------------------------- IF (CONDICIONAL)-----------------------------------

void condicional(funcao f, char* line, int*cont){
  int val;
  char tipo;
  printf("dgasuda\n");
  sscanf("if %ci%d", &tipo, &val);

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

  printf("jnz after%d\n", *cont);
  *cont ++;
}
  
// ---------- MAIN-------------------------------------------------------------------------------------
int main()
{
  char v1;
  char p1,p2,p3,f,t1,t2,t3;
  int r, i1, i2;
  int inicio, fim, passo, contador_if = 0;
  char line[LINESZ],temp[LINESZ];
  int count = 0, bloco = 0;
  void *usr;
  funcao f1;
  
  printf(".section .rodata\n.data\n\n.text\n\n");
  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL) {
    count++;
    remove_newline(line);

    // ---------------------------- END ------------------------------------------
    if (strncmp(line, "end", 6) == 0) {           // printa leave ret e torna os valores de f1 default
      printf("leave\nret\n\n");
      f1.qtd_var = 0;
      strcpy(f1.str, "");
      f1.tam_pilha = 24;
      f1.variavel = NULL;
      continue;
    }

    
    // -------------------- CABEÇALHO FUNÇÃO----------------------------------------
    r = sscanf(line, "function f%c p%c1, p%c2, p%c3", &f, &p1, &p2, &p3);
    
    if (r >= 2) {
      cabecalho_funcao(f, &f1);
      continue;
    }
    
    // -------------------------------IF (TEM QUE FAZER)----------------------------------------------
    if(strncmp(line,"if",2) == 0) condicional(f1, line, &contador_if);

    if(strncmp(line, "endif", 5) == 0) printf("after%d:\n", contador_if);

    //----------------------------------------DEF----------------------------------------
    if(strncmp(line,"def",3) == 0) {
      bloco = 1;
      continue;
    }


    
    // -----------------------------BLOCO DE DEFINIÇÃO DE VARIAVEL--------------------------
    if(bloco == 1){

      // ----------------DECLARAÇÃO INT--------------------------------
      if(strncmp(line,"var",3) == 0){
        declaracao_int(&f1,line);
        continue;
      }

      // --------------DECLARAÇÃO ARRAY-----------------------------------
      if(strncmp(line,"vet",3)==0){
        declaracao_vet(&f1, line);
        continue;
      }

      // ------------------------ENDDEF------------------------------------------
      if(strncmp(line,"enddef",6)==0){
        bloco = enddef(&f1);
        continue;
      }
    }



    //------------------------OPERAÇÕES COM AS VARIAVEIS--------------------------------------------
    if(line[1] == 'i' || line[1] == 'a'){  

      if(line[8] == 'l'){

        call_function(&f1,line);
        continue;

      }
      operacoes(line,f1);
      continue;
    }

    //-----------------------SETAR VALOR DE ELEMENTO DO ARRAY---------------------------------------
    if(strncmp(line,"set",3) == 0){

      set_array(f1,line);
      continue;
      
    }


    //----------------------PEGAR ELEMENTO DE UM ARRAY------------------------------------------
    if(strncmp(line, "get", 3) == 0){

      get_array(f1,line);
      continue;
    
    } 


    //-----------------------------RETURNS-----------------------------------------------------
    // retorna constante
    if(sscanf(line, "return ci%d",&i1) == 1){ 
      printf("  movl $%d, %%eax\n",i1);
      continue;

    }
    
    // retorna parametro
    if(strncmp(line, "return p",8) == 0){   

      sscanf(line,"return p%c%d",&p1,&i1);
      if(i1 == 1) {                 
        printf("  movl %%edi, %%eax\n");            // se for o primeiro parametro
        continue;
      }
      if(i1 == 2) {
        printf("  movl %%esi, %%eax\n");            // se for o segundo parametro
        continue;
      }
      if(i1 == 3) {
        printf("  movl %%edx, %%eax\n");            // se for o terceiro parametro
        continue;
      }
    }

    // retorna variavel
    if(strncmp(line, "return v", 8) == 0){
      sscanf(line, "return vi%d", &i1);
      printf("  movl %d(%%rbp), %%eax\n\n", f1.variavel[i1]);
    }  

  }

  return 0;
}
