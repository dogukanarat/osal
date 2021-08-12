#include "os_socket.hpp"
#include "os_memory.hpp"

OS_socket_address_in_t *OS_socket_address_in_init( uint16_t d_port_number_u16 )
{

    OS_socket_address_in_t *p_socket_address_in_st = (OS_socket_address_in_t*)OS_mem_allocate( sizeof( OS_socket_address_in_t ) );

    OS_mem_set(p_socket_address_in_st, 0, sizeof(*p_socket_address_in_st));

    p_socket_address_in_st->sin_family      = AF_INET;
    p_socket_address_in_st->sin_addr.s_addr = htonl(INADDR_ANY);
    p_socket_address_in_st->sin_port        = htons(d_port_number_u16);

    return p_socket_address_in_st;

}

int OS_socket_tcp( )
{
    return socket(AF_INET, SOCK_STREAM, 0);
}
