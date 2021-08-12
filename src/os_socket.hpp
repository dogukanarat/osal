#include <netdb.h>
#include <netinet/in.h>

typedef struct sockaddr_in OS_socket_address_in_t;

typedef socklen_t OS_socket_length_t;

OS_socket_address_in_t *OS_socket_address_in_init( uint16_t d_port_number_u16 );

int OS_socket_tcp( );