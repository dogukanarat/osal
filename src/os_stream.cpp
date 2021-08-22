#include "os_stream.hpp"
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>


/**
 * @brief It flush the std::cout buffer
 * 
 */
void OS_flush()
{
    flush(std::cout);
}

/**
 * @brief It prints the buffer on the screen
 * 
 * @param p_string_buf_u8 
 */
void OS_display( const char *p_string_buf_u8 )
{
    std::cout << p_string_buf_u8 << std::endl;
    
    OS_flush();
}

/**
 * @brief 
 * 
 * @param format 
 * @param ... 
 */
void OS_print( const char *format, ...)
{
    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf (stdout, format, arg);
    va_end (arg);
}