#include "bbuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>



int currentThreads; // a count of the current threads in buffer
sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void* buffer[BUFFER_SIZE];

void bbuff_init(){
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);
  pthread_mutex_init(&mutex, NULL);
  currentThreads = 0;
  return;
}


void bbuff_blocking_insert(void* item){
  sem_wait(&empty); // lock the semaphore for 'empty'
  pthread_mutex_lock(&mutex);
  if (currentThreads < BUFFER_SIZE){
    buffer[currentThreads] = item;
    currentThreads++;

pthread_mutex_unlock(&mutex);
    sem_post(&full);

    return;
  }else {

      pthread_mutex_unlock(&mutex);
        sem_post(&full);
    return; //buffer is full
  }

  }

  void* bbuff_blocking_extract(){

    sem_wait(&empty);
      pthread_mutex_lock(&mutex);
    void* temp;
    if (currentThreads > 0){
      temp = buffer[currentThreads - 1];
      currentThreads--;

      pthread_mutex_unlock(&mutex);
            sem_post(&empty);
      return temp;
    } else {

        pthread_mutex_unlock(&mutex);
            sem_post(&empty);
      return NULL;
    }
  }

  _Bool bbuff_is_empty(){
    if (currentThreads == 0){
      return true;
    } else {
      return false;
    }
  }
