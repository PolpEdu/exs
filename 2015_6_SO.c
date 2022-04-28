

/*
Pretende-se criar um conjunto de processos que contem a ocorrência de diferentes números (de 0 a 9) numa matriz. A matriz a
considerar encontra-se em memória partilhada e é inicializada por um outro processo (processo gerador). Cada processo conta
um número específico que lhe é atribuído na altura da sua criação. Depois de todas as contagens serem efectuadas, o processo
gerador volta a inicializar a matriz com novos números de 0 a 9. Este processo é repetido 3 vezes, após as quais todos os
processos são terminados e os recursos usados eliminados.
*/
/*
Mapeie a memória partilhada, onde a matriz e as variáveis auxiliares estão guardadas.
• Crie todos os processos necessários (são no total 11 - um por número de 0 a 9 a procurar e um adicional para gerar os
números da matriz) .
• O processo gerador inicializa a matriz com números de 0 a 9 e dá a cada matriz um número sequencial diferente.
• Cada processo apenas percorre cada matriz uma vez. Para isso, tem de verificar previamente o número sequencial da
matriz existente em memória partilhada para não repetir contagens da mesma matriz. Se já tiver contado a matriz
existente e o número de matrizes ainda não tiver chegado a 3, dorme durante 1s.
• Os processos, após cada contagem, escrevem no ecrã o número de ocorrências encontradas.
• O processo gerador está bloqueado até que todos os outros processos terminem as contagens.
• O processo principal espera que todos os processos filho terminem, limpa todos os recursos usados e sai.
• Declare todas as variáveis que precisa no inicio e use todos os mecanismos de sincronização necessários a um bom
funcionamento do programa
*/

//* OBJETIVO: Pretende-se criar um conjunto de processos que contem a ocorrência de diferentes números (de 0 a 9) numa matriz *//

#include <stdio.h>     // printf
#include <stdlib.h>    // for NULL
#include "semlib.h"    //para ajudar a usar semáforos (SYSv)
#include <sys/wait.h>  //para esperar pelos filhos (wait.h)
#include <sys/shm.h>   //shared memory
#include <sys/types.h> //tipos tipo pid_t
#include <unistd.h>    //para fork

#define DEBUG 1 // testing purposes

#define TOTAL_MATRICES 3 // número de matrizes a calcular
// declaração de variáveis globais

typedef struct mem_shared
{
    int matrix[10][10];
    int n_processed;   // n.o de algarismos já processados
    int matrix_number; // n.o identificador da matriz gerada – cada matriz é gerada
    // com um n.o sequencial diferente
} mems;

mems *mems_ptr;

// Função a ser executada pelo processo gerador that initializes the matrix in shared memory
//! Processo Inicial!!
int main_process()
{

    // initialize the matrix in shared memory with values from 0 to 9  randomly and gives to each matrix a different sequential number
    int i, j;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            mems_ptr->matrix[i][j] = rand() % 10;
        }
    }
    mems_ptr->n_processed = 0;
    mems_ptr->matrix_number++;

#ifdef DEBUG
    // print the matrix created
    printf("\n\nMatrix created:\n");
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            printf("%d ", mems_ptr->matrix[i][j]);
        }
        printf("\n");
    }
#endif
}
// Função a ser executada pelos processos que contam as ocorrências dos algarismos
int count_process(int number_to_count, int matrix_number)
{
    // count the number of occurrences of the number_to_count in the matrix
    int i, j, count = 0;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (mems_ptr->matrix[i][j] == number_to_count)
            {
                count++;
            }
        }
    }

    mems_ptr->n_processed++;

    // print the number of occurrences of the number_to_count in the matrix
    printf("Number %d found %d times in matrix: %d\n", number_to_count, count, matrix_number);

    // check if already counted this matrix and the number of matrixes counted isnt 3, sleep for 1s
    if (matrix_number <= mems_ptr->n_processed && mems_ptr->n_processed != TOTAL_MATRICES)
    {
        printf("Sleeping... Already counted matrix nr: %d\n", matrix_number);
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    // inicializações
    int shmid;
    pid_t idp;

    // Criação e mapeamento da memória partilhada
    // using #include <sys/shm.h>:
    shmid = shmget(IPC_PRIVATE, sizeof(mems), 0666 | IPC_CREAT); // Obtains an identifier to an existing shared memory or creates a new one
    mems_ptr = (mems *)shmat(shmid, NULL, 0);                    // Maps a certain shared memory region into the current process address  space.

    // Initialize the shared memory
    mems_ptr->n_processed = 0;
    mems_ptr->matrix_number = 0;

    // TODO: Criar o processo gerador e os processos que vão efectuar a contagem (cada processo vai contar um número de 0-9)
    // A matriz aconsiderar encontra se em memória partilhada e é inicializada por um outro processo(processo gerador).
    // Cada processo conta um número específico que lhe é atribuído na altura da sua criação.
    // O processo gerador é o primeiro a ser criado e é responsável por inicializar a matriz e atribuir os números aos processos filhos.

    // Criação do processo gerador
    int i = 0;
    for (i = 0; i < TOTAL_MATRICES; i++)
    {
        idp = fork(); // going to fork
        if (idp == 0)
        {
            main_process();
        }
        else
        {
            count_process(rand() % 10, i);
        }
    }

    // TODO: esperar pelo fim dos processos
    // (esperar pelo fim dos processos filho)
    wait(NULL);

    // TODO: remover recursos
    shmdt(mems_ptr);               // Unmaps a certain shared memory region from the current address space.
    shmctl(shmid, IPC_RMID, NULL); // Removes a shared memory segment, because we pass IPC_RMID.

    return 0;
}
