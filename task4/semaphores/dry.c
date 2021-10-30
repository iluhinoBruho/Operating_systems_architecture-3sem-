#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


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


key_t create_key(const char* filename){
	return ftok(filename, 1);
}


int main(){
	const char* file_fkey = "createfile";
	const char* stop_signal = "exit"; //should set it for the ENV
	const char* file_crsp = "dry_info.in";

	FILE* table = fopen("table.txt", "r");
	if(table == NULL){
		perror("table file opening");
		return -2;
	}

	key_t key = create_key(file_fkey);
	if(key < 0){
		perror("key creating");
		fprintf(table, "%s\n", stop_signal);
		return -3;
	}

	struct pair corresp[MAXTYPES]; 
	int num_types = form_corresp(corresp, file_crsp);
	if(num_types < 0){
		perror("file with correspondance");
		return -2;
	}


	int sem_id = semget(key, 1, IPC_CREAT|0660);
	if(sem_id < 0){
		perror("semget");
		return -3;
	}

	struct sembuf inc_space = {0, 1, 0};
	char arg[MAXTYPELEN];
	int cnt = 0;
	while(2021){
		while(fscanf(table, "%s", arg) != EOF){
			if(strcmp(arg, stop_signal) == 0){
				printf("FINISHED (DRY)\n");
				fclose(table);
				return 0;
			}
			int delay  = get_time(corresp, num_types, arg);
			if(delay < 0){
				perror("Negative delay");
				return -4;
			}
			if(semop(sem_id, &inc_space, 1) < 0){
				perror("semop");
				return -5;
			}
			cnt++;
			printf("Drying %s\n", arg);
			sleep(delay);
			printf("Dried\n");
		}
		fclose(table);
		table = fopen("table.txt", "r");
		//to sync 
		for(int i = 0; i < cnt; ++i)    //should consider openind from pos==cnt
			fscanf(table, "%s", arg);
	}
	printf("FINISHED (DRY)");
	fclose(table);
	return 0;
}

