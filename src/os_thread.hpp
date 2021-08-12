#include "pthread.h"

typedef pthread_t OS_thread_t;

void OS_thread_create ( OS_thread_t *p_thread_handler_st, void *(*p_thread)(void *) );

void OS_thread_join   ( OS_thread_t d_thread_handler_st );