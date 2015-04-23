/*
 * stack.c
 *
 *  Created on: Apr 13, 2015
 *      Author: oscar
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"


void StackInit(stackT *stackP, int maxSize){
	stackElemenT *newContents;

	newContents = (stackElemenT *)malloc(sizeof(stackElemenT)*maxSize);

	if (newContents == NULL){
		fprintf(stderr, "Insufficient memory to init stack.\n");
		exit(1);
	}

	stackP -> contents = newContents;
	stackP -> maxSize = maxSize;
	stackP -> top = -1;
}

void StackDestroy(stackT *stackP){
	free(stackP -> contents);
	stackP -> contents = NULL;
	stackP -> maxSize = 0;
	stackP -> top = -1;
}

int StackIsEmpty(stackT *stackP){
	return stackP -> top < 0;
}

int StackIsFull(stackT *stackP){
	return stackP -> top >= stackP -> maxSize - 1;
}

void StackPush(stackT *stackP, stackElemenT element){
	if(StackIsFull(stackP)){
		fprintf(stderr, "Can't push to stack: Stack is full.\n");
		exit(1);
	}

	stackP -> contents[++stackP->top] = element;
}

stackElemenT StackPop(stackT *stackP){
	if (StackIsEmpty(stackP)){
		fprintf(stderr, "Can't pop from stack: Stack is empty.\n");
		exit(1);
	}

	return stackP -> contents[stackP->top--];
}
