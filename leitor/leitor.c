#include <stdio.h>
#include <string.h>
#include "operacoes.c"

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

// ------------------------- CABEÇALHO DA FUNÇÃO-----------------------------
void cabecalho_funcao (char * line, funcao* func){
  char f;
  sscanf(line, "function f%c", &f);
  printf(".globl f%c\nf%c:\n  pushq %%rbp\n  movq %%rsp, %%rbp\n",f,f); 

  func->tam_pilha = 24;       // seta tamanho da pilha inicial igual a 3*8 que são os parametros
  func->qtd_var = 0;        
}

// ---------------------- ENDDEF------------------------------------
int enddef(funcao* f1){     // comandos após as definições de variáveis
  int temp;


  if(f1->tam_pilha % 16 != 0){    // se o tamanho da pilha não for múltiplo de 16

    temp = f1->tam_pilha % 16;    // temp = o resto da divisão do tamanho da pilha por 16
    f1->tam_pilha += temp;        // e adiciona esse resto ao tamanho da pilha, a pilha agora é multiplo de 16

  }

  printf("  subq $%d, %%rsp\n\n", f1->tam_pilha); // printa a alocação da pilha

  return 0;
}
 
// ---------- MAIN-------------------------------------------------------------------------------------
int main()
{
  int  contador_if = 0, bloco_def = 0;
  char line[LINESZ];
  funcao f1;
  
  printf(".section .rodata\n.data\n\n.text\n\n");
  // Lê uma linha por vez
  while (fgets(line, LINESZ, stdin) != NULL) {
    remove_newline(line);

    // ---------------------------- END ---------------------------------
    if (strncmp(line, "end", 6) == 0) {           // printa leave ret e torna os valores de f1 default
      f1.qtd_var = 0;
      f1.tam_pilha = 24;
      free(f1.variavel);
      continue;
    }

    
    // -------------------- CABEÇALHO FUNÇÃO---------------------------------
    if(strncmp(line,"function",8) == 0){
      cabecalho_funcao(line, &f1);
    }
    
    // ------------------------------IF----------------------------------------
    if(strncmp(line,"if",2) == 0) condicional(f1, line, contador_if);

    //------------------------ENDIF-----------------------------------------
    if(strncmp(line, "endif", 5) == 0) {
      printf("after%d:\n", contador_if);
      contador_if++;
    }

    //----------------------------------------DEF-------------------------------
    if(strncmp(line,"def",3) == 0) {
      bloco_def = 1;
      continue;
    }


    
    // --------------------BLOCO DE DEFINIÇÃO DE VARIAVEL-------------------
    if(bloco_def == 1){

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

      // ------------------------ENDDEF---------------------------------------
      if(strncmp(line,"enddef",6)==0){
        bloco_def = enddef(&f1);
        continue;
      }
    }



    //------------------------OPERAÇÕES COM AS VARIAVEIS---------------------
    if(line[1] == 'i' || line[1] == 'a'){  

      if(line[8] == 'l'){     // line[8] = l quando for uma chamada de função

        call_function(&f1,line);
        continue;

      }
      atribuicoes(line,f1); // operações aritmeticas
      continue;
    }

    //-----------------------SETAR VALOR NO ARRAY----------------------------
    if(strncmp(line,"set",3) == 0){

      set_array(f1,line);
      continue;
      
    }


    //----------------------PEGAR ELEMENTO DE UM ARRAY-------------------------
    if(strncmp(line, "get", 3) == 0){

      get_array(f1,line);
      continue;
    
    } 


    //-----------------------------RETURNS---------------------------------------
    if(strncmp(line, "return", 6) == 0){
      returns(line, f1);
    }
      
  }

  return 0;
}
