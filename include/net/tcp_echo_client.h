#include <iostream>
#include <net/tcp_client.h>

#ifndef __TCP_ECHO_CLIENT_H__
#define __TCP_ECHO_CLIENT_H__

namespace m_net {

class EchoClient {
public:
  EchoClient() {}
  ~EchoClient() {}
  int run();
};
} // namespace m_net

#endif
