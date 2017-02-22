#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "tads.h"

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

float target1= 20;    //frequencia desejada para i1<i2
float target2= 10;    //frequencia desejada para i1>i2


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


  	for (i=IMIN;i<=IMAX;i+=STEP){
  		for(int j=IMIN;j<=IMAX;j+=STEP){
  			if(i!=j)
  				count++;
  		}
  	}
    printf("TESTES: %d\n",count);

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
  		//Avalia os 2 robos
  		if(fitness[x]==0){
  			for (i1=IMIN;i1<=IMAX;i1+=STEP){
  				for(i2=IMIN;i2<=IMAX;i2+=STEP){
  					if(i1!=i2){
  							fitness[x]+=evaluate(x,i1,i2)/count;
  					}
  				}
  			}
  		}
  		if(fitness[y]==0){
  			for (i1=IMIN;i1<IMAX;i1+=STEP){
  				for(i2=IMIN;i2<IMAX;i2+=STEP){
  					if(i1!=i2){
  							fitness[y]+=evaluate(y,i1,i2)/count;
  					}
  				}
  			}
  		}
      if (t%5==0)
        printf(".");fflush(stdout);

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
  			printf("\n%6d %2d %5.2f %5.2f %5.2f\n",t,best,fitness[best],fitpopulation/POP,fitness[W]);
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
  float eval;
  Neuron network [NETSIZE]; //declaracao da rede auxiliar
  initialize(network);         //inicializa a rede normalmente
  for (int i=0; i<NETSIZE; i++){      //subscreve os valores da matriz S
    for(int j=0; j<NETSIZE; j++){     //com os valores geneticos
      network[i].S[j]= gene[n][m];
      m++;
    }
  }

  eval=execute(network, inputa, inputb,0);
  
  if (eval==0){ //caso seja 0;
    return 0;
  }
  else if(inputa<inputb){                                     //avalia para casos onde i1 < i2
    if(eval>(target1+target2)/2){                             //se erro
      return 100-(fabs(target1-eval)*3);                      //penaliza
    }
    return 100-fabs(target1-eval); //retorna 100 - a "distancia" entre a frequencia ideal e a obtida
  } else if(inputa>inputb){                                   //avalia para casos onde i1 > i2
    if(eval<(target1+target2)/2){
      return 100-(fabs(target2-eval)*3);                        //penaliza
    }
    return 100-fabs(target2-eval); //ou seja, caso a frequencia desejada seja alcancada retorna 100
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
          if(m==0|| m==n){
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
