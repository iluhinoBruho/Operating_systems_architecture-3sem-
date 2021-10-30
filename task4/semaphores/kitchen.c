#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(){
	int fd = open("table.txt", O_WRONLY|O_CREAT|O_TRUNC, 0660);
	if(fd < 0){
		perror("creating table");
		return -1;
	}
	close(fd);
	pid_t p = fork();
	if(p > 0){
		execl("./wash.exe", "./wash.exe", NULL);
		perror("execl");
		return -1;
	}else if(p == 0){
		execl("./dry.exe", "./dry.exe", NULL);
		perror("execl");
		return -2;
	}else{
		perror("fork");
		return -3;
	}
}
