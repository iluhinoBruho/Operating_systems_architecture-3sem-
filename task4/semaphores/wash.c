#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAXTYPES 128
#define MAXTYPELEN 64


struct pair{
	int time;
	char type[MAXTYPELEN];
};


//would be beter to implement hash_map to have O(1)
//for operation of getting execuction time 
//for eac dish type
int get_time(const struct pair* corresp, int sz, char* arg){
	for(int i = 0; i < sz; ++i){
		if(strcmp(corresp[i].type, arg) == 0)
			return corresp[i].time;
	}
	return -1; //nonexisting type
}

int form_corresp(struct pair* corresp, const char* file_name){
	FILE* in = fopen(file_name, "r");
	if(in == NULL)
		return -1;
	int t, size = 0;
	while(fscanf(in, "%s : %d", corresp[size].type, &(corresp[size].time)) != EOF){
		++size;
		if(size > MAXTYPES){
			perror("too many types");
			return -1;
		}
	}
	fclose(in);
	return size;
}
 

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

key_t create_key(const char* filename){
	return ftok(filename, 1);
}


int main(){
	const char* file_fkey = "createfile";
	const char* file_crsp = "wash_info.in";
	const char* stop_signal = "exit"; //should set it for the ENV
	int TABLE_LIMIT = 16; //set_table();

	struct pair corresp[MAXTYPES]; 
	int num_types = form_corresp(corresp, file_crsp);
	if(num_types < 0){
		perror("file with correspondance");
		return -2;
	}

	if(TABLE_LIMIT < 0){
		perror("error setting the table");
		return -1;
	}

	FILE* table = fopen("table.txt", "w");
	if(table == NULL){
		perror("table file opening");
		return -2;
	}
	FILE* data = fopen("dishes.in", "r");
	if(data == NULL){
		perror("dishes file opening");
		return -2;
	}
	key_t key = create_key(file_fkey);
	if(key < 0){
		perror("key creating");
		fprintf(table, "%s\n", stop_signal);
		return -3;
	}

	//setting semaphores
	int sem_id = semget(key, 1, IPC_CREAT | 0660);
	if(sem_id < 0){
		perror("sem creating");
		fprintf(table, "%s\n", stop_signal);
		return -4;
	}

	struct sembuf semedge;  //to perform operations on semaphores
	semedge.sem_num = 0;
	semedge.sem_op = TABLE_LIMIT;
	semedge.sem_flg = 0;
	struct sembuf dec_space = {0, -1, 0};

	if(semop(sem_id, &semedge, 1) < 0){
		perror("semop");
		fprintf(table, "%s\n", stop_signal);
		return -5;
	}

	char arg[MAXTYPELEN];
	int cnt;
	while(fscanf(data, "%s : %d", arg, &cnt) != EOF){
		int delay = get_time(corresp, num_types, arg);
		if(delay < 0){
			perror("negative delay time");
			return -6;
		}
		for(int i = 0; i < cnt; ++i){
			sleep(delay);
			if(semop(sem_id, &dec_space, 1) < 0){
				perror("semop");
				fprintf(table, "%s\n", stop_signal);
				fflush(table);
				return -5;
			}
			printf("done %s\n\n", arg);
			fprintf(table, "%s\n", arg);
			fflush(table);
		}
	}
	printf("FINISHED\n");
	fprintf(table, "%s\n", stop_signal);
	fflush(table);
	fclose(table);
	fclose(data);
	int status;
	wait(&status);
	return 0;
}

