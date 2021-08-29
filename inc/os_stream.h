#ifndef _OSAL_STREAM_HPP
#define _OSAL_STREAM_HPP

#include <stdint.h>

void OS_flush();

void OS_display( const char *p_string_buf_u8 );

void OS_print( const char *format, ...);

#endif