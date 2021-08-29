#include "os_socket.h"
#include "os_memory.h"

OS_socket_address_in_t *OS_socket_address_in_init( uint16_t d_port_number_u16 )
{

    OS_socket_address_in_t *p_socket_address_in_st = (OS_socket_address_in_t*)OS_mem_allocate( sizeof( OS_socket_address_in_t ) );

    OS_mem_set(p_socket_address_in_st, 0, sizeof(*p_socket_address_in_st));

    p_socket_address_in_st->sin_family      = AF_INET;
    p_socket_address_in_st->sin_addr.s_addr = htonl(INADDR_ANY);
    p_socket_address_in_st->sin_port        = htons(d_port_number_u16);

    return p_socket_address_in_st;

}

OS_socket_address_in_t *OS_socket_address_in_init_by_name( OS_host_t *host, uint16_t d_port_number_u16 )
{

    OS_socket_address_in_t *p_socket_address_in_st = (OS_socket_address_in_t*)OS_mem_allocate( sizeof( OS_socket_address_in_t ) );

    OS_mem_fill_with_zeros( p_socket_address_in_st, sizeof(*p_socket_address_in_st) );

    OS_mem_copy( host->h_addr, &p_socket_address_in_st->sin_addr.s_addr, host->h_length);

    p_socket_address_in_st->sin_family      = AF_INET;
    p_socket_address_in_st->sin_port        = htons(d_port_number_u16);

    return p_socket_address_in_st;

}

int OS_socket_tcp( )
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

int OS_bind( int socket, OS_socket_address_t* socket_address, OS_socket_length_t socket_length )
{
    return bind( socket, socket_address, socket_length);
}

int OS_listen( int socket )
{
    return listen( socket, 3 );
}

int OS_accept( int socket, OS_socket_address_t* socket_client, OS_socket_length_t* socket_length)
{
    return accept( socket, socket_client, socket_length );
}

int OS_read( int socket, uint8_t* buffer, int buffer_length )
{
    return read( socket, buffer, buffer_length);
}

int OS_close( int socket )
{
    return close( socket );
}

int OS_connect( int socket, OS_socket_address_in_t *socket_server )
{
    return connect( socket, (struct sockaddr *)socket_server, sizeof(OS_socket_address_t));
}

int OS_write( int socket, uint8_t* buffer, int buffer_length )
{
    return write( socket, buffer, buffer_length );
}