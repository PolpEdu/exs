

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
#include <semaphore.h> // sem_open, sem_close, sem_wait, sem_post, sem_setvalue
#include <sys/wait.h>  //para esperar pelos filhos (wait.h)
#include <sys/shm.h>   //shared memory
#include <sys/types.h> //tipos tipo pid_t
#include <unistd.h>    //para fork
#include <fcntl.h>     //para usar o O_CREAT
#include <sys/mman.h>  //para mapear a memória partilhada

#define TOTAL_MATRICES 3 // número de matrizes a calcular
// declaração de variáveis globais

//#define DEBUG 1

typedef struct mem_shared
{
    int matrix[10][10];
    int n_processed;   // n.o de algarismos já processados
    int matrix_number; // n.o identificador da matriz gerada – cada matriz é gerada
    // com um n.o sequencial diferente
} mems;

mems *mems_ptr;
// ponteiro para a SM

// semaforos a utilizar
sem_t *worker_sem;
sem_t *gerador_sem;

// Função a ser executada pelo processo gerador that initializes the matrix in shared memory
//! Processo Inicial!!
int gerador()
{

    // initialize the matrix in shared memory with values from 0 to 9  randomly and gives to each matrix a different sequential number
    int i, j, t;
    for (t = 0; t < TOTAL_MATRICES; t++)
    {
        //! Going to change shared memory matrix. Perform a wait in the semaphore.
        sem_wait(gerador_sem);
        mems_ptr->n_processed = 0;
        mems_ptr->matrix_number = t + 1;
        for (i = 0; i < 10; i++)
        {

            for (j = 0; j < 10; j++)
            {

                mems_ptr->matrix[i][j] = rand() % 10;
            }
        }
#ifdef DEBUG
        // print the matrix created
        printf("\n\nMatrix nr %d created:\n", mems_ptr->matrix_number);
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 10; j++)
            {
                printf("%d ", mems_ptr->matrix[i][j]);
            }
            printf("\n");
        }
#endif
        sem_post(worker_sem); // signal to the worker that the matrix is ready, unlock the worker_sem
    }

    exit(0);
}
// Função a ser executada pelos processos que contam as ocorrências dos algarismos
int worker(int number_to_count)
{
    int i, j, counter = 0;
    int matrizatual = 0;

    while (1)
    {
        //! Wait in the semaphore.
        sem_wait(worker_sem); // we are going to change the shared memory matrix. So we need to wait in the semaphore.
        if (matrizatual == mems_ptr->matrix_number)
        {
            sem_post(worker_sem); // signal to the worker that the matrix is ready, unlock the worker_sem
            if (matrizatual == TOTAL_MATRICES)
            {
                exit(0); // we can close the worker processes because we have counted all the matrices
            }
            sleep(1);
        }
        else
        {
            counter = 0;
            for (i = 0; i < 10; i++)
            {
                for (j = 0; j < 10; j++)
                {
                    if (mems_ptr->matrix[i][j] == number_to_count)
                    {
                        counter++;
                    }
                }
            }

            mems_ptr->n_processed++;               // increment the number of processed numbers
            matrizatual = mems_ptr->matrix_number; // update the current matrix number
            if (mems_ptr->n_processed == 10)       // if all the matrices have been processed
            {
                sem_post(gerador_sem); // unlock the gerador_sem
            }
            sem_post(worker_sem); // signal to the worker that the matrix is ready, unlock the worker_sem
            printf("Counted %d occurrences of %d in matrix nr %d\n", counter, number_to_count, mems_ptr->matrix_number);
        }
    }
}

int main(int argc, char *argv[])
{
    // inicializações
    int shmid;
    pid_t idp;

    // Criação e mapeamento da memória partilhada
    // using #include <sys/shm.h>:
    shmid = shmget(IPC_PRIVATE, sizeof(mems), 0700 | IPC_CREAT); // Obtains an identifier to an existing shared memory or creates a new one
    mems_ptr = (mems *)shmat(shmid, NULL, 0);                    // Maps a certain shared memory region into the current process address  space.

    // cria um named semaphore
    worker_sem = sem_open("sem1", O_CREAT | O_EXCL, 0700, 0);  // cria um named semaphore com o nome "sem1"
    gerador_sem = sem_open("sem2", O_CREAT | O_EXCL, 0700, 1); // cria um named semaphore com o nome "sem2"

    // Initialize the shared memory
    mems_ptr->n_processed = 0;
    mems_ptr->matrix_number = 0;

    // TODO: Criar o processo gerador e os processos que vão efectuar a contagem (cada processo vai contar um número de 0-9)
    // A matriz aconsiderar encontra se em memória partilhada e é inicializada por um outro processo(processo gerador).
    // Cada processo conta um número específico que lhe é atribuído na altura da sua criação.
    int i = 0;
    for (; i < 11; i++)
    {
        idp = fork();
        if (idp == 0)
        {
            // child process
            if (i == 0)
            {
                gerador(); // gerador
            }
            else
            {
                worker(i - 1); // create a worker for each number in the 3 matrices.
            }
            exit(0);
        }
    }

    // TODO: esperar pelo fim dos processos
    // (esperar pelo fim dos processos filho)
    for (i = 0; i < 11; i++) // 11 processes, 11 waits.
    {
        wait(NULL);
    }

    // TODO: remover recursos
    sem_close(worker_sem);  // close the semaphore
    sem_close(gerador_sem); // close the semaphore
    sem_unlink("sem1");     // detach the named semaphore
    sem_unlink("sem2");     // detach the named semaphore

    shmdt(mems_ptr);               // Unmaps a certain shared memory region from the current address space.
    shmctl(shmid, IPC_RMID, NULL); // Removes a shared memory segment, because we pass IPC_RMID.

    return 0;
}
