/*
 * stack
 *
 *  Created on: Apr 13, 2015
 *      Author: oscar
 */

#ifndef STACK_H_
#define STACK_H_


typedef char stackElemenT;

typedef struct {
	stackElemenT *contents;
	int top;
	int maxSize;
} stackT;


void StackInit(stackT *stackP, int maxSize);

void StackDestroy(stackT *stackP);

int StackIsEmpty(stackT *stackP);

int StackIsFull(stackT *stackP);

void StackPush(stackT *stackP, stackElemenT element);

stackElemenT StackPop(stackT *stackP);




#endif /* STACK_ */
