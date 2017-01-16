#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "analysis.h"

#define POP 30           //popn size
#define LEN NETSIZE*NETSIZE           //genotypes
#define MUT 0.05          // mutn prob at each locus
#define REC 0.6          //recomb prob at each locus 0.6
#define writeBestGene 1 //Save the best genotype of the simulation in a file
#define writePopulation 1 //Write the last population of a simulation in a file

bool loadPopulationFromFile; //Load all individuals genotype from file
bool logDynamic; //Save the dynamic of a individual (usuallly the best)
bool loadBestGeneFromFile; //Load only the best  from the last simulation
int  trials;        //Trials for each individual
int  end;
int  start;		//start value for logGA (column "iteration")
float fitness[POP];      //Fitness of each individual
float gene[POP][LEN];    //global array containing the binary genotypes of popn

float target1= 80;    //frequencia desejada para i1<i2
float target2= 40;    //frequencia desejada para i1>i2
float imin = 3;       //valor minimo de Teste
float imax = 33;      //valor maximo de teste
float istep = 1;      //especo entre as entradas de teste.


void init_pop();
void write_pop(int best);
float evaluate (int n,float inputa, float inputb);
void read_conf_file();




int main(int argc, char const *argv[]) {

  	srand(time(NULL));
  	int i,t,x,y,W,L,best=0;
  	time_t t1,t2; (void) time(&t1);
    //Calcula o tempo de execucao
  	srand48(time(0)); srand (time(0));

  	int count=0;

  	for (i=imin;i<=imax;i+=istep){
  		for(int j=imin;j<=imax;j+=istep){
  			if(i!=j)
  				count++;
  		}
  	}

  	printf("Testes: %d\n",count);


    //inicia o fitness da população
  	for (int k=0;k<POP;k++)
  		fitness[k]=0;


  	read_conf_file(); //Leitura do arquivo de configuração da simulação

  	init_pop();//inicializa a população



  	FILE * file;
  	file = fopen("logGA.txt","w");//Armazena evolução do GA
  	t=0;
  	//Escolhe os dois primeiros robôs
  	x=POP*drand48();
  	y=POP*drand48();
  	for (t=start;t<end+start;t++){
    float i1, i2;
  		//Avalia os 2 robs
  		if(fitness[x]==0){
  			for (i1=imin;i1<=imax;i1+=istep){
  				for(i2=imin;i2<=imax;i2+=istep){
  					if(i1!=i2){
  							fitness[x]+=evaluate(x,i1,i2)/count;
  					}
  				}
  			}
  		}
  		if(fitness[y]==0){
  			for (i1=imin;i1<imax;i1+=istep){
  				for(i2=imin;i2<imax;i2+=istep){
  					if(i1!=i2){
  							fitness[y]+=evaluate(y,i1,i2)/count;
  					}
  				}
  			}
  		}


  		//Verifica qual foi o vencedor
  		if (fitness[x] > fitness[y])  {W=x; L=y;}
  		else                          {W=y; L=x;}

  		//Altera os parâmetros (gene) do perdedor
  		for ( i=0; i<LEN; i++){
        if(gene[L][i]!=0){            //garante as "desconexoes" continuem
          if (drand48()<REC)
          gene[L][i]=gene[W][i];
          if (drand48()<MUT){
            gene[L][i]+=(0.3*drand48())-0.15;
            if (gene[L][i]>1) gene[L][i]-=1;
            else if (gene[L][i]<0) gene[L][i]+=1;
          }
        }
  		}

  		//reinica finess do perdedor para que ele seja
  		//avaliado novamente
  		fitness[L]=0;

  		//Escolhe outros 2 indivíduos
  		x=POP*drand48();
  		do{
  			y=POP*drand48();
  		}while(y==x);

  		if (t%100 ==0 || t+1==end+start){
  			float fitpopulation;
  			fitpopulation=fitness[0];
  			best=0;
  			//Atualiza o melhor indivíduo e o fitness da população.
  			for (int k=1;k<POP;k++){
  				if (fitness[best]<fitness[k])
  					best=k;
  				fitpopulation+=fitness[k];
  			}
  			fprintf(file,"%6d %2d %.2f %.2f\n",t,best,fitness[best],fitpopulation/POP);
  			printf("%6d %2d %5.2f %5.2f %5.2f\n",t,best,fitness[best],fitpopulation/POP,fitness[W]);
  			write_pop(best);
  		}
  	}
  	logDynamic=true;
  	printf("\nBest individual's (%d) internal dynamic saved - fitness: %9.5f",best, fitness[best]);


  	write_pop(best);
  	fclose(file);
  	(void) time(&t2);
  	printf("\nRunning time: %ld\n",t2-t1);
  	return 0;



  return 0;
}


float evaluate(int n, float inputa, float inputb){

  int m=0;
  Neuron network [NETSIZE]; //declaracao da rede auxiliar
  initize(network);         //inicializa a rede normalmente

  for (int i=0; i<NETSIZE; i++){      //subscreve os valores da matriz S
    for(int j=0; j<NETSIZE; j++){     //com os valores geneticos
      network[i].S[j]= gene[n][m];
      m++;
    }
  }



}


/*Initialise the population at random or loading from file.*/
void init_pop() {
	int i,j;
	if (loadPopulationFromFile) //Load the population from file
	{ FILE * popfile = fopen("logPopulation.txt","r");
		for (int i=0;i<POP;i++)
		{    for (int j=0;j<LEN;j++)
					 fscanf(popfile,"%f",&(gene[i][j]));
		}
		fclose(popfile);
	}
	else
	{
		for (i=0;i<POP;i++){
      for (j=0;j<LEN;j++)
				   gene[i][j]=drand48(); //Number between 0 and 1
			fitness[i]=0;
      j=0;
      for(int m=0; m<NETSIZE;m++){        //"Gambiarra" para garantir o neuronio 0
        for(int n=0; n<NETSIZE; n++){     //desconectado assim com a diagonal da matriz R
          if(m==0||n==0|| m==n){
            gene[i][j]=0;
          }
          j++;
        }
      }

		}
		if (loadBestGeneFromFile) //Load only the best gene from another simulation
		{	 FILE * bestGenefile = fopen("logBestGene.txt","r");
			 for(int n=0;n<LEN;n++)
				 fscanf(bestGenefile,"%f",&(gene[0][n])); //Store in position 0
				//printf(" %7.5f ",gene[0][n]);
				fclose(bestGenefile);
		}
	}
}


void write_pop(int best){
	if (writeBestGene)
	{ FILE * bestGenefile = fopen("logBestGene.txt","w");
		for ( int i=0; i<LEN; i++)
			fprintf(bestGenefile,"%8.6f ",gene[best][i]);
		fclose(bestGenefile);
	}
	if (writePopulation)
	{ FILE * popfile = fopen("logPopulation.txt","w");
		for (int i=0;i<POP;i++){
		    for (int j=0;j<LEN;j++){
			     fprintf(popfile,"%8.6f ",gene[i][j]);
			  }
	     fprintf(popfile,"\n");
	  }
		fclose(popfile);
	}
}

void read_conf_file(){
  char *cmd;
  char buf[200];
  FILE *arq;
  arq = fopen("config.txt","r");
 if (arq == NULL){
    printf("\nErro ao abrir arquivo de configuracao\n");
    return;
  }
  fgets(buf,200,arq);
  while (!feof(arq))
  {
    cmd = strtok(buf,"=");
    int val = atoi(strtok(NULL,";"));
		if (strcmp(cmd,"evolution") == 0)
			if (val==0)
			{
				loadPopulationFromFile=false;
				 trials=1;
				 loadBestGeneFromFile=true;
				 logDynamic=true;
				 break;
			}
		if (strcmp(cmd,"loadPopulationFromFile") == 0)
			loadPopulationFromFile=val;
		if (strcmp(cmd,"trials") == 0)
			trials=val;
		if (strcmp(cmd,"logDynamic")==0)
			logDynamic=val;
		if (strcmp(cmd,"loadBestGeneFromFile")==0)
			loadBestGeneFromFile=val;
		if (strcmp(cmd,"end")==0)
			end=val;
		if (strcmp(cmd,"start")==0)
			start=val;
    fgets(buf,200,arq);

  }
  //printf("%d %d %d %d\n",loadPopulationFromFile,trials,logDynamic,loadBestGeneFromFile);
  fclose(arq);
}
