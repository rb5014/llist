#ifndef _LLIST_INTERNAL_H_
#define _LLIST_INTERNAL_H_

#include "llist.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

/* Node structure */
typedef struct __list_node {
    void *data;
    struct __list_node *next;
} list_node_t;

/* Main list structure */
struct __list {
    list_node_t *head;
    unsigned int size;
    int thread_safe;
#ifdef _WIN32
    CRITICAL_SECTION lock;
#else
    pthread_mutex_t lock;
#endif
};

#endif /* _LLIST_INTERNAL_H_ */