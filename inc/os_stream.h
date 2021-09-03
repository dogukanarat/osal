#ifndef _OSAL_STREAM_HPP
#define _OSAL_STREAM_HPP

namespace OSAL
{
    class Stream
    {
        public:
        static void flush();
        static void display( const char *p_string_buf_u8 );
        static void print( const char *format, ...);
    };

}

#endif