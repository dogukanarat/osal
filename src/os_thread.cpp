#include "os_thread.h"

/**
 * @brief It creates a thread
 * 
 * @param p_thread_handler_st 
 * @param p_thread 
 */
void OS_thread_create ( OS_thread_t *p_thread_handler_st, void *(*p_thread)(void *) )
{
    pthread_create( p_thread_handler_st, NULL, p_thread, NULL );
}

/**
 * @brief It wait the thread to be executed by blocking
 * 
 * @param d_thread_handler_st 
 */
void OS_thread_join   ( OS_thread_t d_thread_handler_st )
{
    pthread_join( d_thread_handler_st, NULL );
}