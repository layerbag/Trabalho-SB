#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINESZ 256

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


typedef struct funcoes{
  int tam_pilha,qtd_var;
  char str[LINESZ];
  int *variavel;
}funcao;

void operacoes(char*line, funcao f){
  int dest,var1,var2;
  char p1,p2,op;
  int r = sscanf(line, "vi%d = %ci%d %c %ci%d",&dest,&p1,&var1,&op,&p2,&var2);

  if(r == 3){             // se for uma atribuição simples do tipo vi1 = vi2
    if(p1 == 'c'){        // se for vi1 = ci1

      printf("  movl $%d, %d(%%rbp)\n\n", var1, f.variavel[dest]);  

    }else{                // se for vi1 = vi2
      printf("  movl %d(%%rbp), %%r10d\n", f.variavel[var1]);
      printf("  movl %%r10d, %d(%%rbp)\n\n",f.variavel[dest]);

    }
  }else if(r > 3){        // se for uma operação mais complexa

    printf("  movl ");    
    if(p1 == 'c'){

      printf("$%d, %%r10d\n", var1);

    }else{

      printf("%d(%%rbp), %%r10d\n", f.variavel[var1]);

    }
    if(op == '+') printf("  addl ");
    if(op == '*') printf("  imull ");
    if(op == '-') printf("  subl ");

    if(p1 == 'c'){

      printf("$%d, %%r10d\n", var2);

    }else{

      printf("%d(%%rbp), %%r10d\n", f.variavel[var2]);

    }

    printf("  movl %%r10d, %d(%%rbp)\n\n", f.variavel[dest]);
  }

}

void set_array(funcao f, char * line){
  int array, index,value;
  char p1;

  int r = sscanf(line,"set va%d index ci%d with %ci%d",&array,&index,&p1,&value);
  printf("  leaq %d(%%rbp), %%r10\n", f.variavel[array]);

  if(p1 == 'c') printf("  movl $%d, %d(%%r10)\n\n", value, index);
  else printf("  movl %d(%%rbp), %%r8d\n  movl %%r8d, %d(%%r10)\n\n", f.variavel[value], index);
  
}

void get_array(funcao f, char* line){
  int array,index,var;
  char p1;
  int r = sscanf(line, "get %ca%d index ci%d to vi%d",&p1, &array, &index, &var);

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
}

void cabecalho_funcao (char f, funcao* func){
  printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f);
  func->tam_pilha = 24;
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

int enddef(funcao* f1){
  int i1;
  char temp[25];
  if(f1->tam_pilha != 0){
    if(f1->tam_pilha % 16 != 0){
      i1 = f1->tam_pilha % 16;
      f1->tam_pilha += i1;
    }
    sprintf(temp, "  subq $%d, %%rsp\n\n", f1->tam_pilha);
    strcat(f1->str, temp);

    printf("%s\n",f1->str);
  }

  return 0;
}

void call_function (funcao*f, char *line){
  char *temp;
  char dest[3];
  char parametros[3][5];
  char *func = malloc(2);
  int param2[3] = {-8,-16,-24};
  int c = -1, index, cont_p = 0;

  strcpy(parametros[0], "%edi");
  strcpy(parametros[1], "%esi");
  strcpy(parametros[2], "%edx");

  temp = strtok(line, " ");
  strcpy(dest,temp);
  printf("  movq %%rdi, -8(%%rbp)\n  movq %%rsi, -16(%%rbp)\n  movq %%rdx, -24(%%rbp)\n\n");

  while(temp != NULL){
    if(c != -1){
      if(temp[0] == 'f'){
        strcpy(func, temp);
      }
      else if(temp[0] == 'c' && temp[1] == 'i'){

        printf("  movl $%d, %s\n", temp[2]-'0', parametros[c]);
        c++;

      }else if(temp[0] == 'v' && temp[1] == 'i'){

        index = temp[2] - '0';
        printf("  movl %d(%%rbp), %s\n",f->variavel[index], parametros[c]);
        c++;

      }else if(temp[0] == 'v' && temp[1] == 'a'){

        index = temp[2] - '0';
        printf("  leaq %d(%%rbp), %%r10\n", f->variavel[index]);
        parametros[c][1] = 'r';
        printf("  movq %%r10, %s\n", parametros[c]);
        parametros[c][1] = 'e';
        c++;

      }else if(temp[0] == 'p' && temp[1] == 'i'){
        
        index = temp[2] - '0';
        printf("  movl %d(%%rbp), %%r10d\n", param2[index]);
        printf("  movl %%r10d, %s\n", parametros[c]);
        c++;

      }else if(temp[0] == 'p' && temp[1] == 'a'){

        index = temp[2] - '0';
        printf("  leaq %d(%%rbp), %%r10\n", param2[index]);
        parametros[c][1] = 'r';
        printf("  movq %%r10, %s\n", parametros[c]);
        parametros[c][1] = 'e';
        c++;
      }
    }
    if(c == -1) c = 0;
    temp = strtok(NULL, " ");
  }

  printf("\n  call %s\n", func);
 
  if(dest[0] == 'v' && dest[1] == 'i'){

    index = dest[2] - '0';
    printf("  movl %%eax, %d(%%rbp)\n\n",f->variavel[index]);

  }else if(dest[0] == 'v' && dest[1] == 'a'){

    index = dest[2] - '0';
    printf("  movq %%rax, %d(%%rbp)\n\n", f->variavel[index]);

  }else if(dest[0] == 'p' && dest[1] == 'i'){
        
    index = dest[2] - '0';
    printf("  movl %%eax, %d(%%rbp)\n\n", param2[index]);

  }else if(dest[1] == 'a'){

    index = dest[2] - '0';
    printf("  movq %%rax, %d(%%rbp)\n\n", param2[index]);
  
  }
  printf("  movq -8(%%rbp), %%rdi\n  movq -16(%%rbp), %%rsi\n  movq -24(%%rbp), %%rdx\n\n");
}
  
// ---------- MAIN-------------------------------------------------------------------------------------
int main()
{
  char v1;
  char p1,p2,p3,f,t1,t2,t3;
  int r, i1, i2;
  int inicio, fim, passo;
  char line[LINESZ],temp[LINESZ];
  int count = 0, bloco = 0;
  void *usr;
  funcao f1;
  
  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL) {
    count++;
    remove_newline(line);

    // ---------------------------- END ------------------------------------------
    if (strncmp(line, "end", 6) == 0) {
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
    
    // -------------------------------IF----------------------------------------------
    r = sscanf(line, "if v%d > v%d", &i1, &i2);
    if (r == 2) {
      printf("Linha %d: %s\n", count, line);
      printf("Indices: %d e %d\n", i1, i2);
      printf("---\n");
      continue;
    }

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
      //------------------------OPERAÇÕES COM AS VARIAVEIS--------------------------------------------
    }else if(line[1] == 'i' || line[1] == 'a'){  

      if(line[8] == 'l'){

        call_function(&f1,line);
        continue;

      }
      operacoes(line,f1);
      continue;

      //-----------------------SETAR VALOR DE ELEMENTO DO ARRAY---------------------------------------
    
    }else if(strncmp(line,"set",3) == 0){

      set_array(f1,line);
      continue;
      //----------------------PEGAR ELEMENTO DE UM ARRAY------------------------------------------
    }else if(strncmp(line, "get", 3) == 0){

      get_array(f1,line);
      continue;
    //-----------------------------RETURNS-----------------------------------------------------
    } 
    if(sscanf(line, "return ci%d",&i1) == 1){ 
      printf("  movl $%d, %%eax\n",i1);
      continue;

    }else if(strncmp(line, "return p",8) == 0){   

      sscanf(line,"return p%c%d",&p1,&i1);
      if(i1 == 1) {
        printf("  movl %%edi, %%eax\n");
        continue;
      }
      if(i1 == 2) {
        printf("  movl %%esi, %%eax\n");
        continue;
      }
      if(i1 == 3) {
        printf("  movl %%edx, %%eax\n");
        continue;
      }
    }else if(strncmp(line, "return v", 8) == 0){
      sscanf(line, "return vi%d", &i1);
      printf("  movl %d(%%rbp), %%eax\n\n", f1.variavel[i1]);
    }  

  }

  return 0;
}
