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



int main(){
	const char* stop_signal = "exit"; 
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
		
		/*
		struct pair corresp[MAXTYPES];
		int num_types = form_corresp(corresp, file_crsp);
		if(num_types < 0){
			perror("dry: file with correspondance");
			return -2;
		}*/

		sleep(10);
		while(1){
			close(table[1]);  //don't write to table
			close(notify[0]);  //don't read from notify
			int str_size;
			if(read(table[0],  &str_size, sizeof(int)) < 0){
				perror("reading from pipe");
				return -6;
			}
			char dish[MAXTYPELEN];
			if(read(table[0], dish, sizeof(char) * str_size) < 0){
				perror("reading from pipe");
				return -6;
			}
			if(strcmp(dish, stop_signal) == 0){
				printf("DRYING DONE\n");
				return 0;
			}
			
			printf("Drying dish: >%s<\n\n", dish);
			fflush(stdout);
			//delay = get_time(corresp, num_types, dish);
			//if(delay < 0){
			//	perror("negative delay time");
			//	return -6;
			//}	
			//sleep(delay);

			sleep(5);
			printf("Dried\n");
			int notification = 1;
			write(notify[1], &notification, sizeof(int));
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
		
		FILE* data = fopen("dishes.in", r);
		
		int free_space = 5; //TABLE_LIMIT;
		while(1)
		{
			close(table[0]); //we don't read from table
			close(notify[1]); //we don't write to notify
			char dish[MAXTYPELEN];
			printf("Input your dish: \n");
			if(fgets(dish, MAXTYPELEN, stdin) < 0){
				perror("read from file.in");
				return -3;
			}
			
			if(strcmp(dish, stop_signal) == 0){
				//also write to table about it
				printf("DONE WASHIG\n");
				return 0;
			}
			
			int str_size = strlen(str);
			str[str_size - 1] = '\0';
			write(table[1],  &str_size, sizeof(int));
			if(write(table[1], str, sizeof(char) * (strlen(str) + 1)) < 0){
				perror("write to pipe");
				return -4;
			}
			free_space--;
			printf("FREESPACE: %d\n\n", free_space);
			if(free_space == 0){
				printf("ZEROSPACE\n");
				int sign_ready;
				int ready = read(notify[0], &sign_ready, sizeof(int));
				if(ready > 0 && sign_ready == 1)
					free_space++;
				printf("ZEROPACE: %d<space %d<sign %d<ready\n\n", free_space, sign_ready, ready);
			}
		}	

	}else{
		perror("fork");
		return -2;
	}
}
