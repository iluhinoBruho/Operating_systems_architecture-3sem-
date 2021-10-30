#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc != 2){
		printf("File not passed or not only the file passed.\n");
		return -1;
	}
	FILE* data;
	if((data = fopen(argv[1], "r")) == NULL){
		printf("Error while opennig file.\n");
		return -2;
	}
	int delay;
	char to_exec[128] = "";
	int children = 0;
	char sep[] = " ";
	int deb = 0;
	pid_t cur_pid = getpid();
	while(fscanf(data, "%d %[^\n]%*c", &delay, to_exec) != EOF){
		pid_t p = fork();
		switch(p){
			case -1:
				perror("fork");
				return -3;
			case 0: ; //semi-colon added after a label
				char* child_arg[512] = {};
				char* part_str = strtok(to_exec, sep);
				int c = 0;
				//printf("to_exec: %s\n", to_exec);
				while(part_str != NULL){
					child_arg[c] = part_str;
					part_str = strtok(NULL, sep); // to continue separating from rememdered x00
					c++;
				}
				//fflush(stdout);
				if(c <= 0){
					perror("empty arg proc");
					return -5;
				}
				child_arg[c] = NULL;
				/*for(int i = 0; i < c; ++i){
					printf("%d : %s\n", i, child_arg[i]);
				}*/
				sleep(delay);
				int proc = execvp(child_arg[0], child_arg);
				if(proc==-1){
					perror("executing child");
					return -4;
				}
			default:
				continue;
		}
	}
	int each;
	for(int i = 0; i < children; ++i)
		wait(&each);	
	fclose(data);
	return 0;
}
