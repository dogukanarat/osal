#ifndef _OSAL_SOCKET_HPP
#define _OSAL_SOCKET_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

namespace OSAL
{
    class Socket
    {
        public:
        typedef struct sockaddr_in address_in_t;
        typedef socklen_t length_t;
        typedef struct sockaddr address_t;
        typedef struct hostent host_t;

        static address_in_t* address_in_init( uint16_t d_port_number_u16 );
        static address_in_t* address_in_init_by_name( host_t *host, uint16_t d_port_number_u16 );

        static int _socketDgram();
        static int _socketStream();
        static int _controlNonBlocking( int socket );
        static int _bind( int socket, address_t* socket_address, length_t socket_length );
        static int _listen( int socket );
        static int _accept( int socket, address_t* socket_client, length_t* socket_length);
        static int _read( int socket, uint8_t* buffer, int buffer_length );
        static int _close( int socket );
        static int _connect( int socket, address_in_t *socket_server );
        static int _write( int socket, uint8_t* buffer, int buffer_length );
    };
}
#endif