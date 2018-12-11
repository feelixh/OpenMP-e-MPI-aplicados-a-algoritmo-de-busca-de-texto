#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "tempo.h"
#include "mpi.h"
#include <omp.h>


int nCaracteres();

int main(int argc,char *argv[])
{
	tempo1();
	MPI_Status status;
	int numprocs, myid;
	

	omp_set_num_threads(numprocs);

	
	MPI_Init(&argc,&argv);

    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

	
	if (myid == 0) //mestre
    {

    
    	int vaiDe = 0;
   		int vemDe = nCaracteres();
   		int parte = nCaracteres()/numprocs;
   		int i, totalTop=0, totalDown=0, numTop=0, numDown=0;
		if (numprocs >1){
    		for (i=1; i< numprocs; i++){ // mestre envia para escravos "i"
				vaiDe = vaiDe + parte;
				vemDe = vemDe - parte;
				MPI_Send(&vaiDe,     1, MPI_INT, i, 4, MPI_COMM_WORLD);
				MPI_Send(&vemDe,     1, MPI_INT, i, 4, MPI_COMM_WORLD);  
				MPI_Send(&parte,     1, MPI_INT, i, 4, MPI_COMM_WORLD);
			}
		}
		
		// COMEÇO MÉTODO TOPDOWN
            FILE *file;
			file = fopen("texto.txt","r");	 

			fseek(file, 0, SEEK_SET); //Reposiciona o indicador de posição do fluxo em função do deslocamento.
			
			char code[4]={"DHGK"}, c, buf[4];

			c=getc(file);  // leitura do primeiro char do arquivo
			buf[0] = c;

			c=getc(file);  // leitura do segundo char do arquivo
			buf[1] = c;

			c=getc(file);  // leitura do terceiro char do arquivo
			buf[2] = c;

			while((c=getc(file)) != EOF && ftell (file)<parte){  // leitura dos n char do arquivo

				buf[3] = c;

				if ((code[0] == buf[0]) 
					&& (code[1] == buf[1]) 
		 				&& (code[2] == buf[2]) 
		 				&& (code[3] == buf[3])){
						
						totalTop ++;
				}
		
				buf[0] = buf[1];
				buf[1] = buf[2];
				buf[2] = buf[3];
		
			}
		
			fseek(file, 0, SEEK_END);
			int sz = ftell(file);
			
			fseek(file, -2L, SEEK_END);
	
			c=getc(file);  // leitura do primeiro char do arquivo
			buf[0] = c;

			fseek(file, -3L, SEEK_END);
	
			c=getc(file);  // leitura do segundo char do arquivo
			buf[1] = c;

			fseek(file, -4L, SEEK_END);
	
			c=getc(file);  // leitura do terceiro char do arquivo
			buf[2] = c;
	
			long count;
			
			for(count = sz-parte;count<=sz;count++){
		
				fseek(file, -count, SEEK_END);
	
				buf[3] = getc(file);

				if ((code[0] == buf[0]) 
			 		&& (code[1] == buf[1]) 
			 			&& (code[2] == buf[2]) 
				 			&& (code[3] == buf[3])){
					
					totalDown ++;
				}
		
				buf[0] = buf[1];
				buf[1] = buf[2];
				buf[2] = buf[3];
		
			}
			
	
		fclose(file);
	
		if (numprocs>1){
			for (i=1; i < numprocs; i++){
				MPI_Recv(&numTop,  1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &status);
                MPI_Recv(&numDown, 1, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &status);
				
				totalTop  = totalTop  + numTop;
				totalDown = totalDown + numDown;
			}	
		}
		printf("Segundo o mestre, a quantidade de vezes que a sequência aparece no texto é de: %d\n",totalDown+totalTop);
		printf("Sendo o TOPDOWN: %d\n", totalTop);
		printf("Sendo BOTTOMUP: %d\n", totalDown);
	

    }else
    { //escravos
    	
        
    		int  vaiTop, vemDown, part;
            MPI_Recv(&vaiTop,       1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
			MPI_Recv(&vemDown,      1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
            MPI_Recv(&part,         1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
            
            // COMEÇO MÉTODO TOPDOWN
            FILE *file;
			file = fopen("texto.txt","r");	 
			
	
			fseek(file, vaiTop, SEEK_SET); //Reposiciona o indicador de posição do fluxo em função do deslocamento.
			
			int numTop =0;

			char code[4]={"DHGK"}, c, buf[4];

			c=getc(file);  // leitura do primeiro char do arquivo
			buf[0] = c;

			c=getc(file);  // leitura do segundo char do arquivo
			buf[1] = c;

			c=getc(file);  // leitura do terceiro char do arquivo
			buf[2] = c;

			while((c=getc(file)) != EOF && ftell (file)<(vaiTop+part)){  // leitura dos n char do arquivo

				buf[3] = c;

				if ((code[0] == buf[0]) 
					&& (code[1] == buf[1]) 
		 				&& (code[2] == buf[2]) 
		 				&& (code[3] == buf[3])){
				numTop ++;
				}
		
				buf[0] = buf[1];
				buf[1] = buf[2];
				buf[2] = buf[3];
		
			}

			// COMEÇO DO MÉTODO BOTTOMUP
			fseek(file, vemDown, SEEK_SET);
			int sz = ftell(file);
			int numDown = 0;
			fseek(file, -2L, SEEK_END);
	
			c=getc(file);  // leitura do primeiro char do arquivo
			buf[0] = c;

			fseek(file, -3L, SEEK_END);
	
			c=getc(file);  // leitura do segundo char do arquivo
			buf[1] = c;

			fseek(file, -4L, SEEK_END);
	
			c=getc(file);  // leitura do terceiro char do arquivo
			buf[2] = c;
	    

		#pragma omp parallel
		{
			long count;
			
			#pragma omp for private (count)
			for(count = sz-part;count<=sz;count++){
		
				fseek(file, -count, SEEK_END);
	
				buf[3] = getc(file);

				if ((code[0] == buf[0]) 
			 		&& (code[1] == buf[1]) 
			 			&& (code[2] == buf[2]) 
				 			&& (code[3] == buf[3])){
					
					numDown ++;
				}
				#pragma omp critical
				buf[0] = buf[1];
				#pragma omp critical
				buf[1] = buf[2];
				#pragma omp critical
				buf[2] = buf[3];
			
			}
		}	
			
		fclose(file);
	
        MPI_Send(&numTop,     1, MPI_INT, 0, 4, MPI_COMM_WORLD);
		MPI_Send(&numDown,    1, MPI_INT, 0, 4, MPI_COMM_WORLD);
		}

   

  MPI_Finalize();

  tempo2();
  tempoFinal("mili segundos", argv[0], 0);
 
  return 0;
 
}

int nCaracteres(){

  FILE *arquivo; // ponteiro para o arquivo
  int tamanho; // tamanho em bytes do arquivo

  // abre o arquivo para leitura
  arquivo = fopen("texto.txt", "r");


  // verifica se o arquivo foi aberto com sucesso
  if (arquivo != NULL) {

    // movimenta a posição corrente de leitura no arquivo para o seu fim
    fseek(arquivo, 0, SEEK_END);

    // pega a posição corrente de leitura no arquivo
    tamanho = (int) ftell(arquivo);

    // retorna o tamanho do arquivo
    return tamanho -1;

  } else {
    printf("Arquivo inexistente ao chamar método tamanhoArquivo");
  }

  return 0;

   
}