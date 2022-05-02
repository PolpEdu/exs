#define PIPE_NAME1 "PIPE1"
#define PIPE_NAME2 "PIPE2"

#include <stdio.h>
#include <pthread.h>
#include <sys/unistd.h>
#include <errno.h>
#include <stdlib.h>

// variáveis globais
int fd_named_pipe[2];
int soma_total=0;


// Outras funções
//De 5 em 5 segundos uma thread responsável pelas estatísticas irá escrever no ecrã o número de leituras realizadas e o 
//valor da variável soma_total.





// Função a ser executada pelas thread que lê dos named pipes
/*A thread que lê dados dos named pipes termina quando tiver lido o valor -1 de qualquer um dos named piped. Depois 
disso, a thread que apresenta as estatísticas apresentará uma última vez as estatísticas e também sairá. Por fim, todos os 
recursos em utilização devem ser destruídos (incluindo o named pipe).*/
void *thread_read_named_pipe(void *arg)
{
    //read from named pipe
    int read_value;
    while(1)
    {
        read(fd_named_pipe[0], &read_value, sizeof(int));
        if(read_value == -1)
        {
            break;
        }
        soma_total += read_value;
    }
}




// Função a ser executada pelas thread que lê e escreve a estatística no ecra
// De 5 em 5 segundos uma thread responsável pelas estatísticas irá escrever no ecrã o número de leituras realizadas e o 
//valor da variável soma_total.
void *thread_write_statistics(void *arg)
{
    int n_leituras=0;
    
    while(1)
    {
        // escrever no ecrã
        printf("[%d] %d. Soma total: %d\n", getpid(),n_leituras, soma_total);
        
        // esperar 5 segundos
        sleep(5);

        n_leituras++;
    }
}


/*
Implemente um processo que leia números inteiros de dois named pipes e que apresente no ecrã a estatística dessas leituras a 
cada 5 segundos. A estatística é constituída pelo número de inteiros lidos e pela sua soma total. Deverá ainda ter em conta as 
seguintes funcionalidades e restrições:
    • Os named pipes devem ser criados pelo processo principal no inicio da sua execução e terão o nome “PIPE1” e “PIPE2”.
    • A leitura de dados dos named pipes é feita através de 1 thread específica (apenas uma!).
    • Os dados recebidos pelos named pipes são inteiros.
    • Após a leitura de um novo inteiro, a thread deverá somar esse valor a uma variável de nome soma_total que conterá a 
cada momento a soma total dos valores lidos de todos os named pipes.
    • De 5 em 5 segundos uma thread responsável pelas estatísticas irá escrever no ecrã o número de leituras realizadas e o 
valor da variável soma_total.
    • A thread que lê dados dos named pipes termina quando tiver lido o valor -1 de qualquer um dos named piped. Depois 
disso, a thread que apresenta as estatísticas apresentará uma última vez as estatísticas e também sairá. Por fim, todos os 
recursos em utilização devem ser destruídos (incluindo o named pipe).
    • Declare todas as variáveis que precisa no inicio do código e use todos os mecanismos de sincronização necessários a um 
bom funcionamento do programa.
    • Não deve existir espera activa
*/

    // gcc –lpthread –D_REENTRANT –Wall fich.c –o fich

int main(void) {
    // criar pipes e terão o nome “PIPE1” e “PIPE2”: if they were unnamed: pipe(channel)
    /*
    Implemente um processo que leia números inteiros de dois named pipes e que apresente no ecrã a estatística dessas leituras a 
cada 5 segundos. A estatística é constituída pelo número de inteiros lidos e pela sua soma total. Deverá ainda ter em conta as 
seguintes funcionalidades e restrições:
*/

    // Creates the named pipe if it doesn't exist yet
    if ((mkfifo(PIPE_NAME1, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
        perror("Cannot create pipe %s", PIPE_NAME1);
        exit(0);
    }
    if ((mkfifo(PIPE_NAME2, O_CREAT|O_EXCL|0600)<0) && (errno!= EEXIST)) {
        perror("Cannot create pipe %s", PIPE_NAME2); //#include <errno.h>
        exit(0);
    }
    
    // Creates the named pipe if it doesn't exist yet
    if ((fd_named_pipe[0] = open(PIPE_NAME1, O_RDONLY)) < 0) { //open pipes
        perror("Cannot open pipe %s", PIPE_NAME1);
        exit(0);
    }
    if ((fd_named_pipe[1] = open(PIPE_NAME2, O_WRONLY)) < 0) { //open pipes
        perror("Cannot open pipe %s", PIPE_NAME2);
        exit(0);
    }

    // Creates the thread that reads from the named pipe
    pthread_t thread_estatistica;
    pthread_create(&thread_estatistica, NULL, thread_write_statistics, NULL);

    // Creates the thread that writes on the screen
    pthread_t thread_leitura;
    pthread_create(&thread_leitura, NULL, thread_read_named_pipe, NULL);



    // Esperar pelo fim das threads e remover recursos
    pthread_join(thread_leitura, NULL); // wait for termination of thread_leitura
    pthread_join(thread_estatistica, NULL); // wait for termination of thread_estatistica
    unlink(PIPE_NAME1); //#include <sys/unistd.h>
    unlink(PIPE_NAME2);
    return 0;
}
