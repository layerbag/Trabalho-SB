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
  char *op;
  int *variavel;
}funcao;

char* operacoes(char*line, funcao f){
  
}
int main()
{
  char v1;
  char p1,p2,p3,f,t1,t2,t3;
  int r, i1, i2;
  int inicio, fim, passo;
  char line[LINESZ],temp[LINESZ];
  int count = 0, bloco = 0;
  funcao f1;
  
  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL) {
    count++;
    remove_newline(line);

    // Verifica se line começa com 'end' (3 letras)
    if (strncmp(line, "end", 6) == 0) {
      if(f1.tam_pilha != 0){
        if(f1.tam_pilha % 16 != 0){
          i1 = f1.tam_pilha % 16;
          f1.tam_pilha += i1;
        }
        sprintf(temp, "  subq $%d, %%rsp\n", f1.tam_pilha);
        strcat(f1.str, temp);
      }
      printf("%s",f1.str);
      printf("leave\nret\n\n");
      continue;
    }

    
    // verifica função
    r = sscanf(line, "function f%c p%c1, p%c2, p%c3", &f, &p1, &p2, &p3);
    
    if (r >= 2) {
      printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f);
      f1.tam_pilha = 0;
      f1.qtd_var = 0;
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
        r = sscanf(line,"var vi%d", &i1);     // pega o numero da variavel
        f1.qtd_var++;                         // inc a quantidade de variaveis da funcao
        f1.tam_pilha += 4;                    // soma +4 no tamanho da pilha
        
        if(f1.qtd_var == 1){
          f1.variavel = malloc(sizeof(int));  // se for o primeiro elemento faz o malloc
        }else{
          realloc(f1.variavel,f1.qtd_var * sizeof(int));  // senão faz o realloc
        }

        f1.variavel[i1] = f1.tam_pilha * -1;    // posição do elemento adicionado na pilha

      }

      // se for um array de int
      if(strncmp(line,"vet",3)==0){
        r = sscanf(line,"vet va%d size ci%d", &i1,&i2);
        f1.qtd_var++;
        f1.tam_pilha += i2*4;

        if(f1.qtd_var == 1){
          f1.variavel = malloc(sizeof(int));  // se for o primeiro elemento faz o malloc
        }else{
          realloc(f1.variavel,f1.qtd_var * sizeof(int));  // senão faz o realloc
        }

        f1.variavel[i1] = f1.tam_pilha * -1;
      }

      // finaliza o bloco
      if(strncmp(line,"enddef",6)==0){
        bloco = 0;
      }

    }else if(line[1] == 'i' || line[1] == 'a'){  //Operações com as variaveis

      //strcat(f1.op, operacoes(line, f1));
        
    }else if(strncmp(line,"return p",8) != 0){
      sscanf(line,"return %d",&i1);
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
     // sscanf(line, "return v%*c%d", i1);
     // sprintf(temp, "movl %d(%%rbp), %eax", f1.pilha[i1].posicao);
    }  

  }

  return 0;
}
