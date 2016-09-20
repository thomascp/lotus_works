

#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef int INT32;
typedef unsigned int UINT32;
typedef char CHAR;

typedef void VOID;
typedef int BOOL;

#define NULL 0

#define TRUE 1
#define FALSE 0


typedef struct list
{
	struct list *prev;
	struct list *next;
} list;


UINT32 int_disable();
UINT32 int_enable();

#endif

