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

typedef struct var{
  int x;
  int * v;
}variavel;

typedef struct parametros{
  int rdi,rsi,rdx;
}parametro;

int main()
{
  char v1;
  char p1,p2,p3,f,t1,t2,t3;
  int r, i1, i2;
  int inicio, fim, passo;
  char line[LINESZ],temp[LINESZ];
  int count = 0, bloco = 0;
  
  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL) {
    count++;
    remove_newline(line);

    // Verifica se line começa com 'end' (3 letras)
    if (strncmp(line, "end", 6) == 0) {
      printf("leave\nret\n");
	    
      continue;
    }

    
    // verifica função
    r = sscanf(line, "function f%c p%c1, p%c2, p%c3", &f, &p1, &p2, &p3);
    
    if (r == 4) {
      printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f);
      /*printf("Linha %d: %s\n", count, line);
      printf("Lido 'f%c' 'p%c1' 'p%c2' 'p%c3'\n",f,  p1, p2, p3);
      printf("---\n");*/
      continue;
    }
    if (r == 3) {
      printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f);
      /*printf("Linha %d: %s\n", count, line);
      printf("Lido 'f%c' 'p%c1' 'p%c2'\n",f, p1, p2) ;
      printf("---\n");*/
      continue;
    }
    if (r == 2) {
      printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f);
      
      /*printf("Linha %d: %s\n", count, line);
      printf("Lido 'f%c' 'p%c1'\n",f, p1) ;
      printf("---\n");*/

      continue;
    }
    
    // Verifica se é um 'if'
    r = sscanf(line, "if v%d > v%d", &i1, &i2);
    if (r == 2) {
      printf("Linha %d: %s\n", count, line);
      printf("Indices: %d e %d\n", i1, i2);
      printf("---\n");
      continue;
    }

    // aciona o valor lógico bloco
    if(strncmp(line,"def",3) == 0) {
      bloco = 1;
      continue;
    }
    
    // BLOCO DE DEFINIÇÃO DE VARIAVEL
    if(bloco == 1){
      // se for um int
      if(strncmp(line,"var",3) == 0){
      r = sscanf(line,"var vi%d", &i1);
        printf("linha %d: %s\n", count, line);
        printf("Lido 'vi%d'\n",i1);
      }

      // se for um array de int
      if(strncmp(line,"vet",3)==0){
       r = sscanf(line,"vet va%d size ci%d", &i1,&i2);
        printf("linha %d: %s\n", count, line);
        printf("Lido 'va%d' size: '%d'\n",i1,i2);
      }

      // finaliza o bloco
      if(strncmp(line,"enddef",6)==0){
        bloco = 0;
      }
    }else{    //Operações com as variaveis
      if(line[1] == 'i' || line[1] == 'a'){
        char*t;
        printf("linha %d: %s\n",count, line);
        t = strtok(line," ");
        i1 = 0;

        while(t != NULL){

          if(t[0] == '+'){
            printf("+");
          }else if(t[0] == '-'){
            printf("-");
          }else if(t[0] == '*'){
            printf("*");
          }else if(t[0] != '='){
            printf("%s",t);
          }

          t = strtok(NULL," ");
    
        }
        printf("\n");
      }else if(strncmp(line,"return",6) == 0){
        if(strncmp(line,"return p",8) != 0){
          sscanf(line,"return %d",&i1);
          printf("  movl $%d, %%eax\n",i1);
        }

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
      }
    }


  }

  return 0;
}
