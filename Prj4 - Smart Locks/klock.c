#include "klock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct{
	long unqID; // thread and lock with a unique id
	int type; // 0 for lock, 1 for thread
} ragInfo;


typedef struct{
	int index;	// index of matrix
	int *ragMatrix;
	ragInfo *info; 
} rag;


pthread_mutex_t mutex; // critical section
rag graph = {0, NULL, NULL}; 


// Modify edges of rag matix, by either inserting or removing edge
void modifyEdgeEdit(long IDoutof, int Typeoutof, long IDinto, int Typeinto, int editType){
	int indexOutof, indexInto;

	for(int i = 0; i < graph.index; i++){
		if(graph.info[i].unqID == IDinto && graph.info[i].type == Typeinto){
			indexInto = i;
		}
		if(graph.info[i].unqID == IDoutof && graph.info[i].type == Typeoutof){
			indexOutof = i;
		}
	}

	// If editType is 0, insert the edge, else remove
	if(editType == 0){
		*(graph.ragMatrix + indexOutof* graph.index + indexInto) = 1;
		printf("Insert [%d][%d]\n", indexOutof, indexInto);
	}else{
		*(graph.ragMatrix + indexOutof* graph.index + indexInto) = 0;
		printf("Delete [%d][%d]\n", indexOutof, indexInto);
	}

}


// Print matrix
void printMatrix(){
	for(int i = 0; i < graph.index; i++) {
		for(int j = 0; j < graph.index; j++) {
			printf("%d ", *(j + i*graph.index + graph.ragMatrix));
		}
		printf("\n");
	}
	printf("\n");
}


// Expand the old matrix
void expandMatrix(int index){
	int *temp = malloc(index * index * sizeof(int));
	for(int i = 0; i < index; i++){
		for(int j = 0; j < index; j++){
			if(i < index-1 && j < index-1){
				memcpy(j + i*index + temp, j + i*(index-1) + graph.ragMatrix, sizeof(int));
			}else{
				*(j + i*index + temp) = 0;
			}
		}
	}
	free(graph.ragMatrix);
	graph.ragMatrix = temp;
}


// Recusion used by cyclecheck
int cycleCheckRecc(int *hit, int index){
	if(hit[index] == 1){
		return 0;
	}else{
		hit[index] = 1;
		for(int i = 0; i < graph.index; i++){
			if( *(i + index*graph.index + graph.ragMatrix) == 1 && cycleCheckRecc(hit, i) == 1){
					return 0;
			}
		}
	}
	return 1;
}


// Check ragMatrix for cycle
int cycleCheck(){
	int i;
	int j = 0;
	for(i = 0; i < graph.index; i++){
		while(j < graph.index && *(j + i*graph.index + graph.ragMatrix) != 1){
			j++;
		}
	}
	if(i != graph.index && j!= graph.index){
		int *mark = malloc(graph.index * sizeof(int));
		printMatrix();
		if(cycleCheckRecc(mark, i) == 1){
			free(mark);
			return 1;
		}
	}
	return 0;
}


void init_lock(SmartLock* lock){
	pthread_mutex_init(&lock->mutex, NULL);
	lock->pid = graph.index;

	graph.info = realloc(graph.info, sizeof(ragInfo) * (graph.index+1));
	graph.info[graph.index].unqID = lock->pid;
	graph.info[graph.index].type = 0;

	graph.index++;
	expandMatrix(graph.index);
	printMatrix();
}




int lock(SmartLock* lock){
	pthread_mutex_lock(&mutex);
	int i = 0;
	int hit = 0;

	// Traverse array for repeating threads
	while(i < graph.index && hit == 0){
		if(graph.info[i].unqID == pthread_self() && graph.info[i].type == 1){
			hit = 1;
		}
		i++;
	}

	// If not found add it and expand matrix
	if(hit == 0){
		graph.info = realloc(graph.info, sizeof(ragInfo) * (graph.index+1));
		graph.info[graph.index].unqID = pthread_self();
		graph.info[graph.index].type = 1;

		graph.index++;
		expandMatrix(graph.index);
	}

	// Insert edge to test for a cycle
	printf("Test : ");
	modifyEdgeEdit(pthread_self(), 1, lock->pid, 0 , 0);
	// If cycle detected, delete the edge and return 0
	if(cycleCheck() == 1){
		printf("Result : ");
		modifyEdgeEdit(pthread_self(), 1, lock->pid, 0 , 1);
		printMatrix();
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	// lock
	if(pthread_mutex_trylock(&lock->mutex) == 0){
		//printf("%lu locking\n", pthread_self());
		printf("Result : ");
		modifyEdgeEdit(pthread_self(), 1, lock->pid, 0, 1);
		modifyEdgeEdit(lock->pid, 0, pthread_self(), 1, 0);
		printMatrix();

		pthread_mutex_unlock(&mutex);
		return 1;
	}

	pthread_mutex_unlock(&mutex);
	return 0;	
}


void unlock(SmartLock* lock){ 
	pthread_mutex_lock(&mutex);

	//printf("%lu unlocking\n", pthread_self());
	pthread_mutex_unlock(&lock->mutex);
	modifyEdgeEdit(lock->pid, 0, pthread_self(), 1, 1);
	printMatrix();

	pthread_mutex_unlock(&mutex);
}


/*
 * Cleanup any dynamic allocated memory for SmartLock to avoid memory leak
 * You can assume that cleanup will always be the last function call
 * in main function of the test cases.
 */
void cleanup(){ 
	free(graph.info);
	free(graph.ragMatrix);
	pthread_mutex_destroy(&mutex);
}