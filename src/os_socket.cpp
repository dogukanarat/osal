#include "os_socket.h"
#include "os_memory.h"

using namespace OSAL;

Socket::address_in_t* Socket::address_in_init( uint16_t d_port_number_u16 )
{

    address_in_t *p_socket_address_in_st = (address_in_t*)Memory::memAllocate( sizeof( address_in_t ) );

    Memory::memSet(p_socket_address_in_st, 0, sizeof(*p_socket_address_in_st));

    p_socket_address_in_st->sin_family      = AF_INET;
    p_socket_address_in_st->sin_addr.s_addr = htonl(INADDR_ANY);
    p_socket_address_in_st->sin_port        = htons(d_port_number_u16);

    return p_socket_address_in_st;

}

Socket::address_in_t* Socket::address_in_init_by_name( host_t* host, uint16_t d_port_number_u16 )
{

    address_in_t* p_socket_address_in_st = (address_in_t*)Memory::memAllocate( sizeof( address_in_t ) );

    Memory::memFillWithZeros( p_socket_address_in_st, sizeof(*p_socket_address_in_st) );

    Memory::memCopy( host->h_addr, &p_socket_address_in_st->sin_addr.s_addr, host->h_length);

    p_socket_address_in_st->sin_family      = AF_INET;
    p_socket_address_in_st->sin_port        = htons(d_port_number_u16);

    return p_socket_address_in_st;

}

int Socket::_socketDgram( )
{
    return socket(AF_INET, SOCK_DGRAM, 0);
}

int Socket::_socketStream( )
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int Socket::_controlNonBlocking( int socket )
{
    // struct timeval tv;
    // tv.tv_sec = 1;
    // tv.tv_usec = 0;
    // return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    // return setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
}

int Socket::_bind( int socket, address_t* socket_address, length_t socket_length )
{
    return bind( socket, socket_address, socket_length);
}

int Socket::_listen( int socket )
{
    return listen( socket, 3 );
}

int Socket::_accept( int socket, address_t* socket_client, length_t* socket_length)
{
    return accept( socket, socket_client, socket_length );
}

int Socket::_read( int socket, uint8_t* buffer, int buffer_length )
{
    return read( socket, buffer, buffer_length);
}

int Socket::_close( int socket )
{
    return close( socket );
}

int Socket::_connect( int socket, address_in_t *socket_server )
{
    return connect( socket, (struct sockaddr *)socket_server, sizeof(address_t));
}

int Socket::_write( int socket, uint8_t* buffer, int buffer_length )
{
    return write( socket, buffer, buffer_length );
}