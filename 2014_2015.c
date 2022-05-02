/*
Dado um array de N_VECTORES vectores de inteiros com N_ELEMENTOS cada um, existente em
memória partilhada, pretende-se criar um programa que some todos os elementos existentes em cada
vector. Para isso será criado um processo “Worker” por cada vector existente, o qual somará os
elementos desse vector. O resultado será adicionado a uma variável contendo a soma total que se
encontra também em memória partilhada.
    • Mapeie a memória partilhada, onde os vectores e a variável com a soma total estão guardados.
    • Crie todos os processos necessários. Cada processo terá de somar o seu resultado à variável em
memória partilhada com o resultado global.
    • No máximo podem estar 2 processos a trabalhar em simultâneo.
    • O processo principal espera que todos os processos filho terminem, imprime o valor existente na
variável com o resultado total que se encontra em memória partilhada e sai.
    • Declare todas as variáveis que precisa no inicio e use todos os mecanismos de sincronização
necessários a um bom funcionamento do programa
*/

/*
    - Como é que a bolsa é fornecida? É nos dado logo o dinheiro ou nós é que gastamos o nosso e depois é que é fornecida... como? De que forma? Como é que é dada?

    - Passaportes é preciso no evento da viagem? Só no avião?




*/


#include <stdio.h>     // printf
#include <stdlib.h>    // for NULL
#include <semaphore.h> // sem_open, sem_close, sem_wait, sem_post, sem_setvalue
#include <sys/wait.h>  //para esperar pelos filhos (wait.h)
#include <sys/shm.h>   //shared memory
#include <sys/types.h> //tipos tipo pid_t
#include <unistd.h>    //para fork
#include <fcntl.h>     //para usar o O_CREAT
#include <sys/mman.h>  //para mapear a memória partilhada

// declaração de variáveis globais
#define N_VECTORS 5
#define N_ELEMENTOS 10

typedef struct mem_shared
{
    int vector[N_VECTORS][N_ELEMENTOS];
    int result;
} mem_shared;

mem_shared *mems_ptr;
sem_t *sem_soma;

// código dos processos “Worker”
// - somar elementos de um vector e escrever o resultado
// - apenas 2 processos podem trabalhar ao mesmo tempo
// - todas os processos adicionam o valor da sua soma à variável com o resultado global existente em memória partilhada
void worker(int line_to_sum)
{
    // declaração de variáveis locais
    int i, sum = 0;
    int *vector;
    int *result;

    while (1)
    {
        sem_wait(sem_soma); // espera pelo semáforo
        // obter o vector a somar
        vector = mems_ptr->vector[line_to_sum];
        // obter o resultado global
        result = &mems_ptr->result;
        // somar elementos do vector
        for (i = 0; i < N_ELEMENTOS; i++)
        {
            sum += vector[i];
        }
        // adicionar o resultado à variável global
        *result += sum;

        printf("Processo %d: soma = %d\n", (int)getpid(), sum);
        sem_post(sem_soma); // liberta o semáforo
    }
}

// main
int main(int argc, char *argv[])
{
    // inicialização
    int i, j = 0;
    int shm_fd;

    // Mapeamento da memória partilhada já existente e definida pela key 1234567
    shm_fd = shmget(/*"/1234567"*/ IPC_PRIVATE, sizeof(mem_shared), 0700 | IPC_CREAT);
    mems_ptr = (mem_shared *)shmat(shm_fd, NULL, 0);

    // criação de semáforo
    sem_soma = sem_open("/sem_soma", O_CREAT | O_EXCL, 0700, 0);

    // Initialize shared memory
    // generate a random vectore and assign it to the shared memory
    for (i = 0; i < N_VECTORS; i++)
    {
        for (j = 0; j < N_ELEMENTOS; j++)
        {
            mems_ptr->vector[i][j] = rand() % 100;
        }
    }
    mems_ptr->result = 0;

    // Criação dos processos
    for (i = 0; i < N_VECTORS; i++)
    {
        if(i % 3 == 0)
        {
            wait(NULL);
            wait(NULL);
        }

        // only two processes can be run at the same time
        if (fork() == 0)
        {
            worker(i);
        }
    }

    // Esperar pelo fim dos processos filho
    for (i = 0; i < N_VECTORS; i++)
    {
        wait(NULL);
    }

    // impressão do resultado final
    sem_wait(sem_soma);
    printf("Resultado final: %d\n", mems_ptr->result);
    sem_post(sem_soma);

    // remover recursos (incluindo a memória partilhada)
    sem_close(sem_soma);
    sem_unlink("/sem_soma");

    shmdt(mems_ptr);
    shmctl(shm_fd, IPC_RMID, NULL);
    // terminação
    return 0;
}