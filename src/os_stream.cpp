#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdint.h>
#include "osal/os_stream.h"

using namespace OSAL;

/**
 * @brief It flush the std::cout buffer
 * 
 */
void Stream::flush()
{
    std::flush(std::cout);
}

/**
 * @brief It prints the buffer on the screen
 * 
 * @param p_string_buf_u8 
 */
void Stream::display(const char *p_string_buf_u8)
{
    std::cout << p_string_buf_u8 << std::endl;
    Stream::flush();
}

/**
 * @brief 
 * 
 * @param format 
 * @param ... 
 */
void Stream::print(const char *format, ...)
{
    va_list arg;
    int done;

    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
}
