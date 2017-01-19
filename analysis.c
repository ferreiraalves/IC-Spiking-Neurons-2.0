#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tads.h"






int main(int argc, char const *argv[]) {

  int menu;

                                            //inicializacao das sementes aleatorias

  srand(time(NULL));
  srand48(time(0)); srand (time(0));

  Neuron network [NETSIZE];                //Declaracao da Rede
  initialize (network);                   //inicializacao dos parametros


  printf("Digite a avaliacao desejada:\n\n");
  printf("1-Manual \n");
  printf("2-Todos os valores \n");
  scanf("%d",&menu);                               //MENUS

  if (menu==1){                                 //OPCAO 1
    float aux1, aux2;                           //OS VALORES DE ENTRADA E SAIDA SAO DADOS
    printf("\n");                               //PELO USUARIO
    printf("Digite o valor da entrada 1:\n");
    scanf("%f",&aux1);
    printf("Digite o valor da entrada 2:\n");
    scanf("%f",&aux2);
    printf("\n");
    printf("Frequencia: %.3f\n",execute(network,aux1,aux2));
  }else if(menu==2){

    FILE * ft = fopen("frequencyTable.txt","w");        //OPCAO 2
                                                        //O PROGRAMA TESTA TODOS OS VALORES
    float i,j;                                          //ESPECIFICADOS
    for(i=IMIN; i<=IMAX; i+=STEP){
      for(j=IMIN; j<=IMAX; j+=STEP){
        if(i==j){
          fprintf(ft, "XXXX ");
        }else{
          fprintf(ft, "%.2f ",execute(network,i,j));
        }
      }
      printf(".");fflush(stdout);
      fprintf(ft,"\n");
    }
    fclose(ft);
  }
  printf("\n");
  printMatrix(network);

  printf("\nAvaliacao concluida\n\n");

  return 0;
}
