#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc,char *argv[])
{
	if(argc == 1)
	{
		exit(1);
	}

	int fds[(argc-1)*2];
	pid_t pids[argc-1];
	
	
	for(int i = 0; i < argc - 1; i++)
	{
		if(i < argc-2) // Pipe if not last command
		{
			pipe(fds + i*2);
			printf("Pipe for %d and %d read fd = %d, write fd = %d\n", i+i%2, i%2+i+1, fds[i+i%2], fds[i%2+1+i]);
		}
		pids[i] = fork();
		
		if(pids[i]<0) //fork failed
		{
			
			fprintf(stderr, "fork\n");
			exit(1);
		}
		else if(pids[i] == 0)
		{
			/*for(int k = 0; k < (i+1)*2; k+=2){
				printf("Pipe for %d and %d read fd = %d, write fd = %d\n", k, k+1, fds[k], fds[k+1]);
			}
			printf("\n");*/
			if(i < argc-2) // Change Write Command if not last
			{
				dup2(fds[(2*i)+1],1);
			}
			if(i > 0) //if not first command, connect to read of prev pipe
			{
				dup2(fds[(2*i)-2],0);
			}

			for(int j = 0; j < 2*(argc - 2); j++) //close fds
			{
				close(fds[j]);
			}
			execlp(argv[i+1],argv[i+1],NULL); //run command
			perror("execlp");
			exit(1);
		}
		if(i > 0)
		{
			close(fds[(2*i)-2]);
		}
		if(i < argc-2) // Change Write Command if not last
		{
			close(fds[2*i+1]);
		}
		
	}

	for(int i = 0; i < (argc-2)*2; i++)
	{
		close(fds[i]);
	}
	for(int i = 0; i < argc-1; i++)
	{
		waitpid(pids[i], NULL, 0);
	}
	return 0;
}
