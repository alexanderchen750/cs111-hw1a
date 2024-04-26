#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int normalRun(char *exe, int readPrevPipe)
{
	int fds[2];
	if(pipe(fds)<0){
		//errorExit("pipe fail");
		perror("pipe failed");
		exit(errno);
	}
	pid_t processID = fork();
	if(processID < 0){
		//fork failed
		perror("fork failed");
		exit(errno);
	}
	else if(processID ==0)
	{
		dup2(readPrevPipe,0);
		dup2(fds[1],1);
		close(fds[0]);
		close(fds[1]);
		execlp(exe,exe,NULL);
		perror("execlp");
        exit(errno);
		
	}
	else{
		close(readPrevPipe);
		close(fds[1]);
		waitpid(processID, NULL, 0);
		return fds[0];
	}
	return -1;
}

int firstRun(char *exe)
{
	int fds[2];
	if(pipe(fds)<0){
		//errorExit("pipe fail");
		perror("pipe failed");
		exit(errno);
	}
	pid_t processID = fork();
	if(processID < 0){
		perror("fork failed");
		exit(errno);
	}
	else if(processID ==0)
	{
		dup2(fds[1],1);
		close(fds[0]);
		close(fds[1]);
		execlp(exe,exe,NULL);
		perror("execlp");
        exit(errno);
		
	}
	else{
		close(fds[1]);
		waitpid(processID, NULL, 0);
		return fds[0];
	}
	return -1;
}

void lastRun(char *exe, int readPrevPipe)
{
	int fds[2];
	if(pipe(fds)<0){
		//errorExit("pipe fail");
		perror("pipe fail");
		exit(errno);
	}
	pid_t processID = fork();
	if(processID < 0){
		perror("fork failed");
		exit(errno);
	}
	else if(processID == 0)
	{
		dup2(readPrevPipe,0);
		close(fds[0]);
		close(fds[1]);
		execlp(exe,exe,NULL);
		perror("execlp");
        exit(errno);
		
	}
	else{
		close(readPrevPipe);
		close(fds[1]);
		waitpid(processID, NULL, 0);
	}

}
int main(int argc,char *argv[])
{
	if(argc < 2)
	{
		errno = EINVAL;
		exit(errno);
	}
	if(argc == 2)
	{
		execlp(argv[1],argv[1], NULL);
		perror("execlp");
        exit(errno);
	}
	int readPrevPipe = firstRun(argv[1]);
	for(int i =2; i <argc-1; i++){
		readPrevPipe = normalRun(argv[2], readPrevPipe);
	}
	lastRun(argv[argc-1],readPrevPipe);
}