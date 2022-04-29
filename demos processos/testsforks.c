#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int i = 0;
// incremnent a variable with forks 11 times
int main()
{
    pid_t idp;
    int i;
    for (i = 0; i < 3; i++)
    {
        idp = fork(); // going to fork
        if (idp == 0)
        {
            printf("[%d] I'm the son!\n", getpid());
        }
        else
        {
            printf("[%d] I'm the father!\n", getpid());
        }
    }
    return 0;
}
