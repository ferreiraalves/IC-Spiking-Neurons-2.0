#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//CONSTANTES

#define NETSIZE 10      //tamanho da Rede
#define TTOTAL 1000      //Numero de interacoes da simulacao
#define T1 50           //Tempo de ativacao da entrada 1
#define T2 200          //Tempo de ativacao da entrada 2
#define T3 300          //Tempo de inicio da analise da Frequencia

#define IMIN 3        //Maximos e minimos para valores de entrada
#define IMAX 33       //Usados somente na avaliacao automatica
#define STEP 1        //Step dado entre os avaliacoes

//ESTRUTURAS

typedef struct Neuron {

  float a;            //taxa de recuperacao
  float b;            //sensibilidade de recuperacao
  float c;            // valor de reset do potencial apos ativacao
  float d;            //valor de recuperacao apos ativacao

  float S [NETSIZE];  //Matriz S (PESOS SINAPTICOS)

  float v;            //potencial de membrana
  float u;            //recuperacao de membrana
  float I;            //Valor de entrada para cada interacao

  float firing;       //variavel de controle de disparo
  float fired;        //variavel de controle de disparo

}Neuron;

//globais
int menu;



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
        if(i==0 || j==0 || i==j){           //gera valores aleatorios
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

float execute (Neuron * network, float input1, float input2){
                                                  //REALIZA A EXECUCAO DA REDE
                                                  //E RETORNA A FREQUENCIA DE SAIDA
  int i,j;                                        //DO NEURONIO 5
  int t;
  float cont=0;


  // saida tempo x neuronio ativado
  FILE *stl = fopen ("spiketimeline.txt", "w");

  for(t=0; t<TTOTAL; t++){

    for (i=0; i<NETSIZE; i++){                //verifica se algum neuronio
      network[i].fired=network[i].firing;     //esta disparando
      if(network[i].fired){
        fprintf(stl, "%d\t%d\n",t,i);        //registra o disparo
      }
      network[i].firing=0;                  //reinicia a variavel de controle
    }

    for(i=0; i<NETSIZE; i++){           //reinica os valores de input
      network[i].I=0;
    }

    if(t>=T1 && t<=T2+100){           //atualiza os valores de entrada
      network[0].I=input1;            //do neuronio 0 de acordo com
    }else if(t>=T2 && t<=T2+100){     //o tempo da simulacao
      network[0].I=input2;
    }


    for(i=0; i<NETSIZE; i++){           //percorre todos os neuronios e atualiza
                                        //os valores

      for(j=0;j<NETSIZE; j++){             //atualiza os valores de entrada
        if(network[j].fired){
          network[i].I+=network[i].S[j];
        }
      }
                                        //atualiza valores de v e u
      network[i].v += 0.25 * ((0.04 * (network[i].v * network[i].v)) + 4.1 * network[i].v + 108 - network[i].u + network[i].I);
      network[i].v += 0.25 * ((0.04 * (network[i].v * network[i].v)) + 4.1 * network[i].v + 108 - network[i].u + network[i].I);
      network[i].u += 0.5 * network[i].a * (network[i].b * network[i].v - network[i].u);


      if (network[i].v >= 30){          //verifica o disparo
        network[i].firing=1;            //atualiza valores caso o disparo ocorra
        network[i].v = network[i].c;
        network[i].u += network[i].d;

        if(i==5 && (t>=T3 && t<=T3+500)){                       //caso a ativacao seja no neuronio 5
          cont++;                                               //contamos a ativacao
        }
      }
    }
  }

  return cont/0.5;                    //retorna a frequencia

}

int main(int argc, char const *argv[]) {

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

  printf("\nAvaliacao concluida\n\n");

  return 0;
}
