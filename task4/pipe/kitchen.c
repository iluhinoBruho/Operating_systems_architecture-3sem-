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
		
		const char* file_crsp = "dry_info.in";
		
		struct pair corresp[MAXTYPES];
		int num_types = form_corresp(corresp, file_crsp);
		if(num_types < 0){
			perror("dry: file with correspondance");
			return -2;
		}

		close(notify[0]);  //don't read from notify
		close(table[1]);  //don't write to table
		while(1){
			
			int str_size;
			int SZint, SZstr;
			if((SZint = read(table[0],  &str_size, sizeof(int))) < 0){
				perror("reading from pipe");
				return -3;
			}
			char dish[MAXTYPELEN];
			if((SZstr = read(table[0], dish, sizeof(char) * str_size)) < 0){
				perror("reading from pipe");
				return -3;
			}
			
			printf("SIZEZ I GOT: >%d-int = '%d<, >%d-str = '%s<\n", SZint, str_size, SZstr, dish);
			
			if(strcmp(dish, stop_signal) == 0){
				printf("DRYING DONE\n");
				return 0;
			}
			
			printf("Got dish: >%s<\n", dish);
			fflush(stdout);
			sleep(10);
			printf("dried");
			fflush(stdout);
			
			
			
			/*
			int delay = get_time(corresp, num_types, arg);
			if(delay < 0){
				perror("negative delay time");
				return -6;
			}	
			
			sleep(delay)
			*/
			
			//write to notify
		
		}
			
		

	}else if(p == 0){
		//WASH Process
		// consider wash.exe call


		/*
		const char* file_crsp = "wash_info.in";

		struct pair corresp[MAXTYPES];
		int num_types = form_corresp(corresp, file_crsp);
		if(num_types < 0){
			perror("wash: file with correspondance");
			return -2;
		}
		*/
		close(notify[1]); //we don't write to notify
		close(table[0]); //we don't read from table
		
		int FREE_SPACE = TABLE_LIMIT;
		
		while(1){
			/*if(FREE_SPACE == 0){
				if(read(table[0], dish, sizeof(char) * str_size) < 0){
					perror("reading from pipe");
					return -3;
				}		
				sleep(1);
				continue;
			}*/
			
			//if(scanf() == EOF) break;
			
			char dish[MAXTYPELEN];
			printf("Iput your string (Cur pid == %d):\n", p);
			if(fgets(dish, MAXTYPELEN, stdin) < 0){
				perror("read from file.in");
				return -3;
			}
			if(strcmp(dish, stop_signal) == 0){
				printf("DONE WASHIG\n");
				return 0;
			}
			
			int str_size = strlen(dish);
			dish[str_size - 1] = '\0';
			if(write(table[1],  &str_size, sizeof(int)) < 0 || write(table[1], dish, sizeof(char) * (str_size + 1)) < 0){
				perror("write to pipe");
				return -4;
			}
			
		}
	}else{
		perror("fork");
		return -2;
	}
}
