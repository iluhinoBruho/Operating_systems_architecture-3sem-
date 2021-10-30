#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int main(){
	int fd[2];
	if(pipe(fd) < 0){
		perror("pipe");
		return -1;
	}
	pid_t p = fork();
	if(p >  0){
		//dry call
		execl("./dry.exe", "./dry.exe", fd[0], fd[1], NULL);
		perror("execl");
		return -1;
	}else if(p == 0){
		execl("./wash.exe", "./wash.exe", fd[0], fd[1], NULL);
		perror("execl");
		return -1;
	}else{
		perror("fork");
		return -2;
	}
}
