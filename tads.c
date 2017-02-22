#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "tads.h"



void initialize (Neuron * network){      //INICIALIZA A REDE

  network[0].a = 0.02;              //Inicializa os parametros do neuronio
  network[0].b = -0.1;              //de entrada
  network[0].c = -55;
  network[0].d = 6;

  int i,j,n;

  for(i=1; i<NETSIZE; i++){
    network[i].a = 0.02;            //Inicializa os parametros dos
    network[i].b = -0.1;            //demais neuronios
    network[i].c = -55;
    network[i].d = 6;
  }

  FILE * bestGenefile = fopen("logBestGene.txt","r"); //tenta ler o arquivo
                                                      //gerado pelo genetico

  for(i = 0; i < NETSIZE; i++){             //inicializa os demais
    network[i].firing = 0;                  //parametros
    network[i].v = -60;
    network[i].u = (network[i].v * network[i].b);
    network[i].I = 0;

    for(j=0 ; j<NETSIZE; j++){
      if(!bestGenefile){                    //caso o arquivo nao tenha sido lido
        if(i==0 || i==j){           //gera valores aleatorios
          network[i].S[j]=0;                //para a matriz S
        }else{
          network[i].S[j]=drand48();
        }
      }else{                                //caso exita le do arquivo
        fscanf(bestGenefile,"%f", &network[i].S[j]);
      }
    }
  }

  if(bestGenefile){
      fclose(bestGenefile);
  }

}

void printMatrix(Neuron * network){  //IMPRIME A MATRIZ S
  int i, j;

  for(i=0; i<NETSIZE; i++){
    for(j=0; j<NETSIZE; j++){
      printf("%.2f ",network[i].S[j]);
    }
    printf("\n");
  }
}

void printTeste(Neuron * network){  //IMPRIME valores especificos dos neuronios
  int i, j;
  printf("\n\n");
  for(i=0; i<NETSIZE; i++){
    printf("N%d - %.2f",i,network[i].v);
    printf("\n");
  }
}

float execute (Neuron * network, float input1, float input2, int genOut){
                                                  //REALIZA A EXECUCAO DA REDE
                                                  //E RETORNA A FREQUENCIA DE SAIDA
  int i,j;                                        //DO NEURONIO 5
  int t;
  float cont=0;
  double G [NETSIZE];
  double tfire [NETSIZE];

  // saida tempo x neuronio ativado
  FILE * stl = fopen ("spiketimeline.txt", "w");
  //sainda tempo x potencial do neuronio 0
  FILE * pt0 = fopen ("potencialn0.txt", "w");
  //saida tempo x potencial do neuroni 5
  FILE * pt5 = fopen ("potencialn5.txt", "w");

  for (i=0;i<NETSIZE;i++){  //inicializa condutancia
    G[i]=0;
    tfire[i]=1000;
  }


  for(t=0; t<TTOTAL; t++){

    for (i=0; i<NETSIZE; i++){                //verifica se algum neuronio
      network[i].fired=network[i].firing;     //esta disparando
      if(network[i].fired){
        if(genOut){
          fprintf(stl, "%d\t%d\n",t,i);        //registra o disparo
        }
      }
      network[i].firing=0;                  //reinicia a variavel de controle
    }

    for(i=0; i<NETSIZE; i++){           //reinica os valores de input
      network[i].I=0;
    }

    if(t>=T1 && t<=T1+100){           //atualiza os valores de entrada
      network[0].I=input1;            //do neuronio 0 de acordo com
    }else if(t>=T2 && t<=T2+100){     //o tempo da simulacao
      network[0].I=input2;
    }

    // printf("Rodada %d\n",t);                   //TESTE DE VALORES
    // for(i=0;i<NETSIZE;i++){
    //   printf("N:%d : %f\n",i,network[i].v);
    // }
    // if(t>50)
    //   getchar();

    for(i=0; i<NETSIZE; i++){           //percorre todos os neuronios e atualiza
                                        //os valores


                                  //SEM CONDUT
      // for(j=0;j<NETSIZE; j++){             //atualiza os valores de entrada
      //   if(network[j].fired){
      //     network[i].I+=network[i].S[j]*SCALE;    //multiplica pelo escalar dos pesos
      //   }
      // }

                                  //COM CONDUT
      for(j=0;j<NETSIZE; j++){             //atualiza os valores de entrada
          network[i].I+=(network[i].S[j]*SCALE)*G[j];    //multiplica pelo escalar dos pesos
      }

                                        //atualiza valores de v e u
      network[i].v += 0.25 * ((0.04 * (network[i].v * network[i].v)) + 4.1 * network[i].v + 108 - network[i].u + network[i].I);
      network[i].v += 0.25 * ((0.04 * (network[i].v * network[i].v)) + 4.1 * network[i].v + 108 - network[i].u + network[i].I);
      network[i].u += 0.5 * network[i].a * (network[i].b * network[i].v - network[i].u);

      if(i==0 && genOut){                               //gera a saida t x potencial do neuronio 0
        fprintf(pt0, "%d\t%f\n",t,network[i].v);
      }

      if(i==5 && genOut){                               //gera a saida t x potencial do neuronio 5
        fprintf(pt5, "%d\t%f\n",t,network[i].v);
      }


      if (network[i].v >= 30){          //verifica o disparo
        network[i].firing=1;            //atualiza valores caso o disparo ocorra
        tfire[i]=0;
        network[i].v = network[i].c;
        network[i].u += network[i].d;

        if(i==5 && (t>=T3 && t<=T3+500)){                       //caso a ativacao seja no neuronio 5
          cont++;                                               //contamos a ativacao
        }
      }
      G[i]=(tfire[i])*(exp(1-((tfire[i]/tcond))))/tcond;  //atualiza os valores da condutancia
      tfire[i]+=1;
    }
  }
  fclose(stl);
  fclose(pt0);
  fclose(pt5);


  return cont/0.5;                    //retorna a frequencia

}
