#include "list.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(int item){
  //malloc mem and sets item value
  struct nodeStruct* new_node = malloc(sizeof(struct nodeStruct));
  new_node -> item = item;
  new_node -> next = NULL;
  return new_node;
}



/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
  //reallocate head ptr to new node
  node -> next = *headRef;
  *headRef = node;
  return;
}


/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
  struct nodeStruct* current = *headRef;

//Base case: empty list
  if(*headRef == NULL){
    *headRef = node;
    return;
  }

//traverse throught LL to tail
  while(current -> next != NULL){
    current = current -> next; //increment LL
  }
  current -> next = node;
  return;
}


/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head){
  struct nodeStruct* current = head;
  int count = 0;
//traverse through LL and count++
  while(current != NULL){
    current = current -> next; //increment LL
    count++;
  }
  return count;
}


/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item){
  struct nodeStruct* current = head;

//base case: empty list
  if(head == NULL){
    return NULL;
  }

//Case else:
  while(current -> item != item && current -> next != NULL){
    current = current -> next;
  }

//found
  if(current -> item == item ){
    return current;
  }
//else
  return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling
 * List_findNode(). If the list contains only one node, the head of the list
 * should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){
  struct nodeStruct* current = *headRef;
  struct nodeStruct* prev = *headRef;

//base case: empty list
  if(*headRef == NULL){
    return;
  }

//Case 1: delete head
  if (current != NULL && current == node){
    *headRef = current -> next;
    free(current); //free mem
    return;
  }

//Case else:
  while (current != NULL && current !=node){
    prev = current;
    current = current -> next; //increment LL
  }

  prev -> next = current -> next; //reallocate head
  free(current); //free mem
  return;
}


/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
 //bubble sort used here
void List_sort (struct nodeStruct **headRef){
  int swap;
  struct nodeStruct* pointer;
  struct nodeStruct* onepointer = NULL;

//base case: empty list
  if(*headRef == NULL){
    return;
  }

  do{
    swap = 0;
    pointer = *headRef;

    while (pointer -> next != onepointer){
      if(pointer -> item > pointer -> next -> item){

        //swapping
        int temp = pointer -> item;
        pointer -> item = pointer -> next -> item;
        pointer -> next -> item = temp;

        swap = 1;
      }
      pointer = pointer -> next; //increment LL
    }
    onepointer = pointer;
  }while(swap);

}
