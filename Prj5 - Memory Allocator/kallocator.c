#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "kallocator.h"
#include "list.h"

struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
    struct nodeStruct* listallocmem;
    struct nodeStruct* listfreemem; 
};

struct KAllocator kallocator;


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    void* ptr = malloc((size_t)kallocator.size);
    kallocator.memory = ptr;

    // Add some other initialization 
    kallocator.listallocmem = NULL;
    kallocator.listfreemem = List_createNode(ptr, _size);
}

void destroy_allocator() {
    free(kallocator.memory);

    // free other dynamic allocated memory to avoid memory leak
    struct nodeStruct *temp;
    while(kallocator.listallocmem != NULL){ //traverse through allocated mem list
        temp = kallocator.listallocmem->next;
        free(kallocator.listallocmem); //free nodes
        kallocator.listallocmem = temp;
    }
    temp = NULL;
    while(kallocator.listfreemem != NULL){ //traverse through free mem list
        temp = kallocator.listfreemem->next;
        free(kallocator.listfreemem); //free nodes
        kallocator.listfreemem = temp;
    }
    temp = NULL;
}

void* kalloc(int _size) {
    void* ptr = NULL;
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory

    if(_size <= 0 || _size > available_memory() || kallocator.size < _size){ //check size, else return NULL
        return ptr;
    }

    struct nodeStruct *freeblock = NULL; //used to store, current if we find a free block
    struct nodeStruct *current = kallocator.listfreemem;

    if(kallocator.aalgorithm == FIRST_FIT){//first fit, 
        while(current != NULL && _size > current->item){ //travese, finds free first free block
            current = current ->next;
        }
        if(_size < current->item){ //if size fits
            freeblock = current; //store current to freeblock
        }

    }else if(kallocator.aalgorithm == BEST_FIT){ //best fit, finds the smallest avaible freeblock
        int temp = 99999;
        int lowest = 99999;
        while(current != NULL){ 
            if(current->item - _size >= 0){ //check _size fits in free block
                temp = current->item - _size; 
            }
            if(temp < lowest){ //check if temp is smaller than lowest
                freeblock = current;
                lowest = temp; //set new lowest to temp
            }
            current = current ->next; 
        }

    }else{ // worstfit
        int temp = 0;
        int greatest = -1;
        while(current != NULL){
            if(current->item - _size >= 0){ //check _size fits in free block
                temp = current->item - _size; 
            }
            if(temp > greatest){ //check if temp is larger than greatest
                freeblock = current;
                greatest = temp; //set new greatest to temp
            }
            current = current ->next;
        }
    }

    if(freeblock == NULL){ //if no blocks are free, return
        return ptr;
    }
    
    if(freeblock->item - _size == 0 ){ //if block is the same size as _size just insert
        List_insertTail(&kallocator.listallocmem, freeblock);
        freeblock->item = freeblock->item - _size;
        ptr = freeblock->ptr;
        List_deleteNode(&kallocator.listfreemem, freeblock);
    }else{ //resize block and insert in allocated memery list
        struct nodeStruct *newnode = malloc(sizeof(struct nodeStruct));
        newnode->item = freeblock->item - _size;
        freeblock->item = freeblock->item - newnode->item;
        newnode->ptr = freeblock->ptr + _size;
        ptr = freeblock->ptr;
        List_insertTail(&kallocator.listallocmem, freeblock);
        List_deleteNode(&kallocator.listfreemem, freeblock);
        List_insertTail(&kallocator.listfreemem, newnode); 
        newnode = NULL;
    }

    freeblock = NULL; //clean up 
    current = NULL;

    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);

    struct nodeStruct *current = kallocator.listallocmem;
    while(current != NULL && _ptr != current->ptr){ //search list for _ptr, to delete current
        current = current -> next;
    }
    if(current == NULL){ // If not found return
        return;
    }
    
    struct nodeStruct *temp = current->next; //Used to link list again after delete current
    List_insertTail(&kallocator.listfreemem, current);
    List_deleteNode(&kallocator.listallocmem, current);
    current = NULL;
    
    current = kallocator.listallocmem; // start from head, locate the gap, link list again
    if(current != NULL){ //corner case
        while(current->next != NULL){
            current = current -> next; //find when current == NULL
        }
    }
    
    if(current != NULL){ 
        current->next = temp; //link tail of current to head of temp
    }else{ //corner case
        kallocator.listallocmem = temp; //link alloc list head to temp
    }
    
    current = NULL; //clean up
    temp = NULL; 

    // testing and checking values
    // int allocated_chunks = List_countNodes(kallocator.listallocmem);
    // int free_chunks = List_countNodes(kallocator.listfreemem);
    // printf("Allocated chunks = %d\n", allocated_chunks);
    // printf("Free chunks = %d\n", free_chunks);
    // printf("-------------------------\n");
}

int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size
    if(available_memory() == 0 || List_countNodes(kallocator.listfreemem) == 0){ 
        return compacted_size; // return 0 if theres no available mem, and no mem chunks
    }

    struct nodeStruct *alloccurrent = kallocator.listallocmem;

    while(alloccurrent != NULL){
        if(alloccurrent->next == NULL ){ //if theres no next node, break
            break;
        }

        // testing and checking values
        // printf("alloccurrent = %p \n", alloccurrent->ptr);
        // printf("alloccurrent->item = %d \n", alloccurrent->item);
        // printf("alloccurren->next = %p \n", alloccurrent->next->ptr);
        // printf("alloccurren->next->item = %d \n", alloccurrent->next->item);
        if(alloccurrent->ptr + alloccurrent->item < alloccurrent->next->ptr){ //check if size of item is smaller than freeblock

            _before[compacted_size] =  alloccurrent->next->ptr; //set befrore ptr

            memmove(alloccurrent->ptr + alloccurrent->item, alloccurrent->next->ptr, alloccurrent->next->item); //shift mem down
            alloccurrent->next->ptr = alloccurrent->ptr + alloccurrent->item; //set new void ptr value

            _after[compacted_size] =  alloccurrent->next->ptr; //set after ptr

            compacted_size++; //increment compacted_size
        }
        alloccurrent = alloccurrent->next; //increment to traverse list
    }
    
    struct nodeStruct *newnode = malloc(sizeof(struct nodeStruct)); //create one big node 
    newnode->item = available_memory(); //store the avaible mem
    newnode->ptr = alloccurrent->ptr + alloccurrent->item;

    struct nodeStruct *temp; //destroy_allocator() algorithim to delete
    while(kallocator.listfreemem != NULL){ //traverse free mem list
        temp = kallocator.listfreemem->next;
        free(kallocator.listfreemem); //delete nodes
        kallocator.listfreemem = temp;
    }
    temp = NULL;

    List_insertTail(&kallocator.listfreemem, newnode); //insert big final node in the back
    newnode = NULL;

    return compacted_size;
}

int available_memory() {
    int available_memory_size = List_sum(kallocator.listfreemem);
    // Calculate available memory size
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = List_sum(kallocator.listallocmem);
    int allocated_chunks = List_countNodes(kallocator.listallocmem);
    int free_size = available_memory();
    int free_chunks = List_countNodes(kallocator.listfreemem);
    int smallest_free_chunk_size = List_smallest(kallocator.listfreemem);
    int largest_free_chunk_size = List_largest(kallocator.listfreemem);
    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);


    // testing and checking values
    // struct nodeStruct* freeblock = kallocator.listfreemem;
    // printf("List freeblocks:\n");
    // while(freeblock != NULL){
    //     printf("Size: %d\n", freeblock->item);
    //     freeblock = freeblock->next;
    // }
    // struct nodeStruct* allocblock = kallocator.listallocmem;
    // printf("List allocblocks:\n");
    // while(allocblock != NULL ){
    //     printf("Size: %d, Value: %p\n", allocblock->item, allocblock->ptr);
    //     allocblock = allocblock->next;
    // }
}



