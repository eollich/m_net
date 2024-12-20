#ifndef __M_NET_UTIL_H__
#define __M_NET_UTIL_H__

#include <arpa/inet.h>

#define M_NET_TCP_PROTO_VER_1 1
#define M_NET_TCP_CMD_INIT 1

namespace m_net {

void *getInAddr(struct sockaddr *sa);
int get_listener_socket(int port);
void hexDump(const uint8_t *buffer, size_t length);
} // namespace m_net

#endif
