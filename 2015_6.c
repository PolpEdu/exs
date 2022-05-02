#define PIPE_NAME1 "PIPE1"
#define PIPE_NAME2 “PIPE2”

#include <stdio.h>

// variáveis globais
int fd_named_pipe[2];
int soma_total=0;


// Outras funções








// Função a ser executada pelas thread que lê dos named pipes






// Função a ser executada pelas thread que lê e escreve a estatística no ecra




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
        perror("Cannot create pipe %s", PIPE_NAME2);
        exit(0);
    }





    // criar threads
    pthread_t thread_leitura;
    pthread_t thread_estatistica;



    // Esperar pelo fim das threads e remover recursos
}
