#ifndef _OSAL_SOCKET_HPP
#define _OSAL_SOCKET_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

typedef struct sockaddr_in OS_socket_address_in_t;

typedef socklen_t OS_socket_length_t;

typedef struct sockaddr OS_socket_address_t;

typedef struct hostent OS_host_t;

OS_socket_address_in_t *OS_socket_address_in_init( uint16_t d_port_number_u16 );

OS_socket_address_in_t *OS_socket_address_in_init_by_name( OS_host_t *host, uint16_t d_port_number_u16 );

int OS_socket_tcp( );

int OS_bind( int socket, OS_socket_address_t* socket_address, OS_socket_length_t socket_length );

int OS_listen( int socket );

int OS_accept( int socket, OS_socket_address_t* socket_client, OS_socket_length_t* socket_length);

int OS_read( int socket, uint8_t* buffer, int buffer_length );

int OS_close( int socket );

int OS_connect( int socket, OS_socket_address_in_t *socket_server );

int OS_write( int socket, uint8_t* buffer, int buffer_length );

#endif