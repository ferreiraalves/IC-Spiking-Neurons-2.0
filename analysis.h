#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

  #define NETSIZE 10      //tamanho da Rede
  #define TTOTAL 1000      //Numero de interacoes da simulacao
  #define T1 50           //Tempo de ativacao da entrada 1
  #define T2 200          //Tempo de ativacao da entrada 2
  #define T3 300          //Tempo de inicio da analise da Frequencia

  #define IMIN 3        //Maximos e minimos para valores de entrada
  #define IMAX 33       //Usados somente na avaliacao automatica
  #define STEP 1        //Step dado entre os avaliacoes

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

  void initialize ();

  void printMatrix();

  float execute ();


#endif
