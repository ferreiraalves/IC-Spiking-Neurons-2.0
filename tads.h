#ifndef _TADS_H_
#define _TADS_H_


#define NETSIZE 20      //tamanho da Rede
#define TTOTAL 1000      //Numero de interacoes da simulacao
#define T1 50           //Tempo de ativacao da entrada 1
#define T2 200          //Tempo de ativacao da entrada 2
#define T3 300          //Tempo de inicio da analise da Frequencia

#define IMIN 10        //Maximos e minimos para valores de entrada
#define IMAX 100       //Usados somente na avaliacao automatica
#define STEP 10        //Step dado entre os avaliacoes

#define SCALE 30        //escalar dos pesos sinapticos

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

void initialize (Neuron * network);

void printMatrix(Neuron * network);

void printTeste(Neuron * network);

float execute (Neuron * network, float input1, float input2);


#endif
