// candykids.c - cmpt300 - A3
// Jeff Wang (301309384) jwa220

//include
#include "bbuff.h"
#include "stats.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>

//#include <time.h>
#include <pthread.h>

//     threads and locks
// #include <signal.h>
// #include <pthread.h>
// #include <fcntl.h>
// #include <semaphore.h>

// Include library
// #include "aux.h"
// #include "bbuff.h"
// #include "stats.h"



typedef struct  {
    int factory_number;
    double time_stamp_in_ms;
} candy_t;


double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}


_Bool stop_thread = false;
void *factory(void *n){
    int id = n; //*((int*) n);
    int wait;
    
    while (!stop_thread){
        wait = rand()%4;
        printf("Factory %d ships candy and waits %ds\n", id, wait);
        candy_t *candy  = (candy_t *) malloc (sizeof(candy_t));
        if (candy != NULL) {
            candy->factory_number = id;
            candy->time_stamp_in_ms = current_time_in_ms();
        } else {
            printf("ERORR: Unable to allocate threads.\n");
            return NULL;
        }
        stats_record_produced(candy->factory_number);
        bbuff_blocking_insert(candy);
        sleep(wait);
    }
    printf("Candy factory %d done.\n", id);
    return;
}


void *consumer(){
    int wait;
    candy_t *candy;
    
    while(true){
        wait = rand()%2;
        candy = bbuff_blocking_extract();
        if (candy != NULL){
            stats_record_consumed(candy->factory_number, current_time_in_ms()-candy->time_stamp_in_ms);
            free(candy);
        }
        sleep(wait);
    }
    return;
}



int main(int argc, char* argv[]){
	// 1.  Extract arguments
	if(argc != 4){ //checking for 3 arguments
        printf("Error: Must have 3 arguments in the format '<#factories> <#kids> <#seconds>'");
		return 0;
	}

    for(int i = 1; i < argc; i++){
        if(atoi(argv[i])<1){ //checking for positive arguments
            printf("Error: Arguments must be a positive integer");
            return 0;
        }
    }

    int factories = atoi(argv[1]);
    int kids = atoi(argv[2]);
    int seconds = atoi(argv[3]);

    // 2.  Initialize modules
    bbuff_init(); //initilaze the buffer module
    stats_init(factories); //statsticis module

    int factory_id[factories];
    pthread_t producers[factories];
    pthread_t consumers[kids];

    // 3.  Launch candy-factory threads
    for (int i = 0; i < factories; i++){
        factory_id[i] = i; // separate reference variables for each factory
        pthread_create(&(producers[i]), NULL ,factory,&(factory_id[i]));
    }

    // 4.  Launch kid threads
    for (int i = 0; i < kids; i++){
        pthread_create(&(consumers[i]), NULL, consumer, NULL);
    }    

    // 5.  Wait for requested time
    for(int i = 0; i < seconds; i++) {
      printf("Time: %ds\n", i);
      sleep(1);
  	}

    // 6.  Stop candy-factory threads
    stop_thread = true;
    for (int i = 0; i < factories; i++){
        pthread_join(producers[i], NULL);
    }

    // 7.  Wait until no more candy
    while(!bbuff_is_empty()) {
    printf("Candy is being consumed...\n");
    sleep(1);
    }

    // 8.  Stop kid threads
    for (int i = 0; i < kids; i++) {
      pthread_cancel(consumers[i]);
      pthread_join(consumers[i],NULL);
    }
    
    // 9.  Print statistics
    stats_display();

    // 10. Cleanup any allocated memory
    stats_cleanup();
    free(producers);
    free(consumers);

    return 0;
}