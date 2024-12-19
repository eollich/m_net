#ifndef __M_NET_TCP_CLIENT_H__
#define __M_NET_TCP_CLIENT_H__

#include <net/tcp.h>

namespace m_net {

class TCPClient : public TCP {
public:
  TCPClient() : TCP() {}
  ~TCPClient() override = default;

  bool setup(const std::string &ip, int port) override;
  bool sendData(const std::string &data);
  std::string receiveData();
};

} // namespace m_net

#endif
