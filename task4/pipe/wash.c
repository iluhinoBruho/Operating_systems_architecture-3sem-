#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


int main(int argc, char* argv[]){
	printf("argc: %d\n", argc);
	int fd[2];
	fd[0] = argv[1];
	fd[1] = argv[2];
	close(fd[0]);
		char str[200];
		pid_t p = getpid();
		printf("Iput your string (Cur pid == %d):\n", p);
		if(fgets(str, 200, stdin) < 0){
			perror("read from file.in");
			return -3;
		}
	int str_size = strlen(str);
	str[str_size - 1] = '\0';
	write(fd[1],  &str_size, sizeof(int));
	if(write(fd[1], str, sizeof(char) * (strlen(str) + 1)) < 0){
		perror("write to pipe");
		return -4;
	}
	close(fd[1]);
	return 0;
}
