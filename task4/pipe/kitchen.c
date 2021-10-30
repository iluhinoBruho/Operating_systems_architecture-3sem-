#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define MAXTYPES 128
#define MAXTYPELEN 64

//some considerations are said thinking of big numbers
//of types & and objects of each type


struct pair{
	int time;
	char type[MAXTYPELEN];
};


//would be beter to implement hash_map to have O(1)
//for operation of getting execuction time 
//for eac dish type


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
	const char* file_crsp = "wash_info.in";
	const char* stop_signal = "exit"; //should set it for the ENV
;
	int TABLE_LIMIT = 16; //set_table(); 
	struct pair corresp[MAXTYPES];
	int num_types = form_corresp(corresp, file_crsp);
	if(num_types < 0){
		perror("file with correspondance");
		return -2;
	}


	for(int i = 0; i < num_types; ++i){
		printf("%d  %s -> %d quants\n", i, corresp[i].type, corresp[i].time);
	}


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
		
		close(notify[0])  //don't read from notify
		close(table[1])	  //don't write to table
		
		
		
		/*sleep(10);
		close(fd[1]);
		int str_size;
		read(fd[0],  &str_size, sizeof(int));
		char str[200];
		read(fd[0], str, sizeof(char) * str_size);
		pid_t p = getpid();
		printf("Got string FST  (Cur pid == %d): %s", p, str);
		fflush(stdout);
		//close(fd[0])

		sleep(5);

		//open(fd[0]);
		close(fd[1]);
		//int str_size;
		read(fd[0],  &str_size, sizeof(int));
		//char str[200];
		read(fd[0], str, sizeof(char) * str_size);
		p = getpid();
		printf("Got string SCND (Cur pid == %d): %s", p, str);
		*/
	}else if(p == 0){
		//WASH Process
		// consider wash.exe call
		
		
		
		
		
		
		
		
		/*
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

		printf("Iput your string (Cur pid == %d):\n", p);
		if(fgets(str, 200, stdin) < 0){
			perror("read from file.in");
			return -3;
		}
		//
		str_size = strlen(str);
		str[str_size - 1] = '\0';
		write(fd[1],  &str_size, sizeof(int));
		if(write(fd[1], str, sizeof(char) * (strlen(str) + 1)) < 0){
			perror("write to pipe");
			return -4;
		}
		close(fd[1]);
		printf("Sent SCND\n");
		*/
	}else{
		perror("fork");
		return -2;
	}
}
