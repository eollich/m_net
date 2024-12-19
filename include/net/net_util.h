#ifndef __M_NET_UTIL_H__
#define __M_NET_UTIL_H__

#include <arpa/inet.h>

#define M_NET_TCP_PROTO_VER_1 1

namespace m_net {

void *getInAddr(struct sockaddr *sa);
int get_listener_socket(int port);
} // namespace m_net

#define M_NET_TCP_CMD_INIT 1

#endif
