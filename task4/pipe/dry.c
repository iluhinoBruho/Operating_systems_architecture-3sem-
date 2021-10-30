#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define MAXTYPES 128
#define MAXTYPELEN 64

struct pair{
	int time;
	char type[MAXTYPELEN];
};

//to store time-costs
//would be beter to implement hash_map to have O(1)
//for operation of getting time by typename
//consideration is said thinking of big numbers
//of types & objects of each type



//func to get time-cost for operation of each type
int get_time(const struct pair* corresp, int sz, char* arg){
	for(int i = 0; i < sz; ++i){
		if(strcmp(corresp[i].type, arg) == 0)
			return corresp[i].time;
	}
	return -1; //nonexisting type
}


//func to form the data for time-cost of operations
int form_corresp(struct pair* corresp, const char* file_name){
	FILE* in = fopen(file_name, "r");

	printf("5\n");
	if(in == NULL)
		return -1;
	int t, size = 0;
	while(fscanf(in, "%s : %d", corresp[size].type, &t) != EOF){
		++size;
		printf("%d <- size\n", size);
		if(size > MAXTYPES){
			perror("too many types");
			return -1;
		}
	}
	fclose(in);
	return size;
}
 

//getting variable from the env
int set_table(){
	char* str = getenv("TABLE_LIMIT");
	if(str == NULL)
		return -1;
	char* end;
	int res = strtol(str, end, 0);  
	if(end == str)
		return -1;
	return res; 	 
}

//forming key
key_t create_key(const char* filename){
	return ftok(filename, 1);
}




int main(){
	const char* file_fkey = "configfile";
	const char* stop_signal = "exit"; //should set it for the ENV

	int TABLE_LIMIT = 16; //set_table(); 
	

	//for bidirectional connection system of 2 pipes
	int notify[2];
	int table[2];
	if(pipe(notify) < 0 || pipe(table) < 0){
		perror("pipe");
		return -1;
	}
	
	
	//the system implementation:
	//we write every washed dish to table_pipe
	//(but not more then free size on the table)
	//each time dry is done with one dish
	//it sends notification to wash proc
	//(inc + 1 to free space on the table)
	pid_t p = fork();
	if(p >  0){
		//DRY process
		//consider dry.exe call 

		sleep(10);
		while(1){
			close(table[1]);
			close(notify[0]);
			int str_size;
			read(table[0],  &str_size, sizeof(int));
			char str[200];
			read(table[0], str, sizeof(char) * str_size);
			pid_t p = getpid();
			printf("Got string (Cur pid == %d): >%s<\n\n", p, str);
			fflush(stdout);
			//close(fd[0])

			sleep(5);
			printf("Dried\n");
		}
		

	}else if(p == 0){
		//WASH Process
		// consider wash.exe call
		
		while(1)
		{
			close(table[0]);
			char str[200];
			pid_t p = getpid();
			printf("Iput your string (Cur pid == %d):\n", p);
			if(fgets(str, 200, stdin) < 0){
				perror("read from file.in");
				return -3;
			}

			int str_size = strlen(str);
			str[str_size - 1] = '\0';
			write(table[1],  &str_size, sizeof(int));
			if(write(table[1], str, sizeof(char) * (strlen(str) + 1)) < 0){
				perror("write to pipe");
				return -4;
			}

			printf("Iput your string (Cur pid == %d):\n", p);
			if(fgets(str, 200, stdin) < 0){
				perror("read from file.in");
				return -3;
			}
			//
			str_size = strlen(str);
			str[str_size - 1] = '\0';
			write(table[1],  &str_size, sizeof(int));
			if(write(table[1], str, sizeof(char) * (strlen(str) + 1)) < 0){
				perror("write to pipe");
				return -4;
		}
		
		printf("Sent SCND\n");
		}
		

	}else{
		perror("fork");
		return -2;
	}
}
