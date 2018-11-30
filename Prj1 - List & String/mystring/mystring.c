#include "mystring.h"
#include <stdio.h>
#include <stdlib.h>

/*
 *   Implement the following functions:
 *
 *   You are NOT allowed to use any standard string functions such as
 *   strlen, strcpy or strcmp or any other string function.
 */

/*
 *  mystrlen() calculates the length of the string s,
 *  not including the terminating character '\0'.
 *  Returns: length of s.
 */
int mystrlen (const char *s)
{
	/* Complete the body of the function */
	int i = 0;
	while (s[i] != '\0'){ //i++ until it reaches the end
		i++;
	}
	return i;
}

/*
 *  mystrcpy()  copies the string pointed to by src (including the terminating
 *  character '\0') to the array pointed to by dst.
 *  Returns: a  pointer to the destination string dst.
 */

char  *mystrcpy (char *dst, const char *src)
{
	/* Complete the body of the function */
	int i = 0;
	while (src[i] != '\0'){
		dst[i] = src[i]; //copy letter by letter from string to new array
		i++;
	}
	dst[mystrlen(src)] ='\0'; //set terminating character at the end
	return dst;
}

/*
 * mystrcmp() compares two strings alphabetically
 * Returns:
 * 	-1 if s1  < s2
 *  	0 if s1 == s2
 *  	1 if s1 > s2
 */
int mystrcmp(const char *s1, const char *s2)
{
	/* Complete the body of the function */
	int i = mystrlen(s1);
	int d = mystrlen(s2);
	int smallest = i;

//find the length of shortest string
	if(i > d){
		smallest = d;
	}

//compare letter by letter
for(int k = 0; k < smallest; k++){
	if(s1[k] < s2[k]){
		return -1;
	}
	if(s1[k] < s2[k]){
		return 1;
	}
}

//if they are the same, check length of string
if(i < d){
	return -1;
}
if(i > d){
	return 1;
}
	return 0;
}

/*
 * mystrdup() creates a duplicate of the string pointed to by s.
 * The space for the new string is obtained using malloc.
 * If the new string can not be created, a null pointer is returned.
 * Returns:  a pointer to a new string (the duplicate)
 	     or null If the new string could not be created for
	     any reason such as insufficient memory.
 */
char *mystrdup(const char *s1)
{
	/* Complete the body of the function */
	char *dst = NULL;
	dst = (char*)malloc(mystrlen(s1)); //malloc mem of new array pt
	int i = 0;
	while (s1[i] != '\0'){
		dst[i] = s1[i]; //copy letter by letter
		i++;
	}
	dst[mystrlen(s1)] = '\0'; //insert terminating character and the end
	return dst;
}
