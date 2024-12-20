#include <net/net_util.h>
#include <net/tcp_client.h>

namespace m_net {
class MNetClient {
public:
  MNetClient(std::string ip, int port) : ip(ip), port(port) {}
  ~MNetClient() {}
  int run(int cmd, std::string cmd_data) {
    TCPClient client;
    if (!client.setup(ip, port)) {
      std::cerr << "Failed to connect to server at " << ip << ":" << port
                << std::endl;
      return 1;
    }
    std::cout << "Connected to server at " << ip << ":" << port << std::endl;

    if (!client.sendData(cmd, cmd_data)) {
      std::cerr << "failed to send data" << std::endl;
      return 1;
    }

    std::cout << "msg sent: " << cmd_data << std::endl;

    std::string response = client.receiveData();
    if (!response.empty()) {
      std::cout << "Received from server: " << response << std::endl;
    } else {
      std::cerr << "Failed to receive response from server" << std::endl;
    }

    client.closeSocket();

    return 0;
  }

private:
  std::string ip;
  int port;
};

} // namespace m_net

// example, should import class and run with branches
// should take args for ip and port too
int main() {
  m_net::MNetClient client("127.0.0.1", 9034);
  // return client.run(M_NET_TCP_CMD_CREATE_ROOM, "a");
  return client.run(M_NET_TCP_CMD_JOIN_ROOM, "a");
}
