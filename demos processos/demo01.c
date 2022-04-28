#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


void filho() {
	  printf("[%d] I'm the son!\n", getpid());
      printf("[%d] My parent is: %d\n", getpid(), getppid());
	}
	
void pai() {
	  wait(NULL);
      printf("[%d] I'm the father!\n", getpid());
	}

int main()
{
  pid_t id;

  id = fork();
  if (id == 0)
    {
      filho();  
    }
  else
    {
	  pai();   
    }
  return 0;
}
