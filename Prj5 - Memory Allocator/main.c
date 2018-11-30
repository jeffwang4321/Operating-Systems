#include <stdio.h>
#include "kallocator.h"

int main(int argc, char* argv[]) {
    initialize_allocator(100, FIRST_FIT);
    //initialize_allocator(100, BEST_FIT);
    //initialize_allocator(100, WORST_FIT);
    printf("Using first fit algorithm on memory size 100\n");

    int* p[50] = {NULL};
    for(int i=0; i<10; ++i) {
        p[i] = kalloc(sizeof(int));
        if(p[i] == NULL) {
            printf("Allocation failed\n");
            continue;
        }
        *(p[i]) = i;
        printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    }

    print_statistics();
    printf("-------------------------\n");

    // for(int i=0; i<10; ++i) {
    //     if(i%2 == 0)
    //         continue;

    //     printf("Freeing p[%d]\n", i);
    //     kfree(p[i]);
    //     p[i] = NULL;
    // }

    for(int i=0; i<10; i=i+2) {

        printf("Freeing p[%d]\n", i);
        printf("hit %d\n", i);
        kfree(p[i]);
        p[i] = NULL;
       
    }

    //check
    for(int i=0; i<10; i++) {
        if( p[i] == NULL ){
            continue;
        }   
        else{
            printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
        }
    }

    print_statistics();
    printf("-------------------------\n");

    // void* before[100] = {NULL};
    // void* after[100] = {NULL};
    // compact_allocation(before, after);

    // void* before[100];
    // void* after[100]; // in this example, total pointers is less than 100
    // printf("p[%d] = %p \n", 2, p[2]);

    // int count = compact_allocation(before, after);
    
    // printf("count: %d\n", count);
    // printf("p[%d] = %p \n", 1, p[1]);
    // for(int i=0; i<count; i++) {
    //     printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
    // }


    void* before[100] = {NULL};
    void* after[100] = {NULL};
    int count = compact_allocation(before, after);

    for( int i = 0; i < count; ++i ){
        for( int j = 0; j < 10; ++j ){
            if( p[j] == before[i] ){
                p[j] = after[i];
                break;
            }
        }
    }

    //check
    for(int i=0; i<10; i++) {
        if( p[i] == NULL ){
            continue;
        }   
        else{
            printf("p[%d] = %p ; *p[%d] = %d\n", i, p[i], i, *(p[i]));
        }
    }

    print_statistics();   

    // You can assume that the destroy_allocator will always be the 
    // last funciton call of main function to avoid memory leak 
    // before exit

    destroy_allocator();

    return 0;
}
