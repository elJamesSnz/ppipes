#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define PROCESS_NUM 3
#define SIZE 20
int matrizA[SIZE][SIZE], matrizB[SIZE][SIZE];int matrizC[SIZE][SIZE];

void llenarM(int val){

    for (int i = 0; i < val; i++)
    {
        
        for (int j = 0; j < val; j++)
        {
            matrizA[i][j] = 1;
            matrizB[i][j] = 1;
        }                      
    }

    for (int i = 0; i < SIZE; i++)
    {
        
        for (int j = 0; j < SIZE; j++)
        {
            if(matrizA[i][j] != 1){
                matrizA[i][j] = 0;
            }

            if(matrizB[i][j] != 1){
                matrizB[i][j] = 0;
            }                       
        }
    }

}

int main(int argc, char* argv[]) {

    if (argv[1] == NULL)
    {        
        printf("Modo de ejcucion ./archivo tamanio_Arreglo\nEjemplo ./pipes 6\n");
        return 1; 
    }

    //array de cuanto x cuanto es la matriz
    int VALOR = 1;
    VALOR = atoi(argv[1]);
    //se rellena la matriz
    llenarM(VALOR);

    int pids[VALOR];


    for (int i = 0; i < VALOR; i++) { //eje x

        for(int j = 0; j<VALOR; j++){ //eje y

            //se establece el pipe
            int fd[2];
            //se valida que no tenga error
            if(pipe(fd) == -1) {
                printf("Error haciendo pipe\n");
                return 1;        
            }

            //se hace fork
            pids[i] = fork();

            //se valida fork
            if (pids[i] == -1) {
                printf("Error creando proceso\n");
                return 1;
            }
            
            if(pids[i] == 0)
            {
                //proceso hijo
                
                int arr[2];
                
                if(read(fd[0], arr, sizeof(int) * 2) < 0){
                    printf("error al leer\n");
                    return 1;
                }
                
                //coordenadas de cierre
                if(arr[0] != -1 && arr[1] != -1)
                {
                    int sum=0;
                     //recibe primer arreglo
                    int v = VALOR*2;
                    int m1[VALOR];
                    
                    if(read(fd[0], m1, sizeof(int) * VALOR) < 0){
                        printf("error en m1\n");
                        return 1;
                    }
                
                    //recibe segundo arreglo
                    
                    int m2[VALOR];
                    if(read(fd[0], m2, sizeof(int) * VALOR) < 0){
                        printf("error en m2\n");
                        return 1;
                    }
                    
                    //se multiplica
                    for(int l=0; l< VALOR; l++){
                        sum = sum + m1[l]*m2[l];
                    }
                    
                    //se envía resultado
                    if (write(fd[1], &sum, sizeof(int)) < 0) {
                        return 1;
                    }                    
                    
                    if (write(fd[1], arr, sizeof(int) * 2) < 0) {
                        return 1;
                    }                                        
                }

                
                close(fd[0]);
                close(fd[1]);
                return 0;                                                        
            }
            else{
                //proceso padre
                int aux[VALOR];
                int aux2[VALOR];
                int coord[2];
                coord[0] = i;
                coord[1] = j;
                

                for(int m=0; m<VALOR; m++){

                    aux[m] = matrizA[i][m];                
                    aux2[m] = matrizB[m][j];
                }
                                
                //se envía arreglo de coordenadas
                if (write(fd[1], coord, sizeof(int) * 2) < 0) {
                    return 1;
                }
                
                //se envía primer arreglo                
                if (write(fd[1], aux, sizeof(int) * VALOR) < 0) {
                    return 1;
                }

                //se envía segundo arreglo                
                if (write(fd[1], aux2, sizeof(int) * VALOR) < 0) {
                    return 1;
                }

                
                wait(NULL);
                //se recupera valor
                int res;
                read(fd[0], &res, sizeof(int));
                //se recuperan coordenadas
                int arr[2];
                read(fd[0], arr, sizeof(int) * 2);


                matrizC[arr[0]][arr[1]] = res;
                               
                close(fd[0]);
                close(fd[1]);            
            }

        }
        
    }

    printf("\nArreglo Final\n");
    for(int i=0; i<VALOR; i++){
        for(int j=0;j <VALOR; j++){
            if(matrizC[i][j] != 0){
                printf("%d ", matrizC[i][j]);
            }  
        }
        printf("\n");
    }

    return 0;
}