#include "os_stream.hpp"
#include<iostream>

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