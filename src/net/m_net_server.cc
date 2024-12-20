#include <net/m_net_server.h>
#include <net/net_util.h>

#define BUFF_SZ 512

// #define M_NET_TCP_CMD_CREATE_ROOM 2
// #define M_NET_TCP_CMD_JOIN_ROOM 3

namespace m_net {

void MNetServer::removeClientByFd(int fd) {
  auto it = std::find_if(clients.begin(), clients.end(),
                         [fd](const pollfd &pfd) { return pfd.fd == fd; });

  if (it != clients.end()) {
    std::cout << "Removing client with fd: " << fd << std::endl;
    close(it->fd);
    clients.erase(it);
  } else {
    std::cerr << "Client with fd " << fd << " not found in clients"
              << std::endl;
  }
}

void MNetServer::handleClientData(int client_fd, const uint8_t *buf,
                                  size_t nbytes) {

  m_net_tcp_header_t *pcktPointer = (m_net_tcp_header_t *)buf;

  // create new packet. send at end

  // extract data
  m_net_tcp_header_t *pckt_pointer = (m_net_tcp_header_t *)buf;
  uint8_t *msg_pointer = NULL;
  uint16_t msg_len = 0;
  processRecvPacket(pckt_pointer, (uint8_t *)buf, &msg_pointer, &msg_len);
  std::string cmd_data(reinterpret_cast<const char *>(msg_pointer), msg_len);

  auto room = rooms.find(cmd_data);
  std::string data = "";

  std::cout << "received request!" << std::endl;
  std::cout << "CMD[" << (int)pcktPointer->cmd << "]" << std::endl;
  std::cout << "CMD DATA[" << cmd_data << "]" << std::endl;
  std::cout << "ROOM EXISTS?[" << (room != rooms.end() ? "Y" : "N") << "]"
            << std::endl;

  m_net_tcp_header_t header;
  uint8_t send_buffer[BUFF_SZ] = {};

  switch (pcktPointer->cmd) {
  case M_NET_TCP_CMD_CREATE_ROOM:
    if (room == rooms.end()) {
      rooms.insert({cmd_data, client_fd});
      initHeader(&header, M_NET_TCP_CMD_CREATE_ROOM, "");
    } else {
      initHeader(&header, M_NET_TCP_CMD_CREATE_ROOM, "",
                 M_NET_TCP_ERROR_ROOM_EXISTS);
    }
    break;
  case M_NET_TCP_CMD_JOIN_ROOM:
    if (room == rooms.end()) {
      initHeader(&header, M_NET_TCP_CMD_CREATE_ROOM, "",
                 M_NET_TCP_ERROR_ROOM_DNE);
    } else { // connect the two peers
             // acctually connect the two guys, for now send back other pfd
      initHeader(&header, M_NET_TCP_CMD_CREATE_ROOM,
                 std::to_string((int)room->second).c_str());
      std::cout << (int)room->second << std::endl;
      data = std::to_string(room->second);
      // removeClient(1);//by fd
      // generate random num for each client. keep track of it for udp ack
    }
    break;
  default:
    break;
  }

  prepareReqPacket(&header, (uint8_t *)data.c_str(), data.size(), send_buffer,
                   sizeof(send_buffer));

  if (send(client_fd, send_buffer, header.len, 0) == -1) {
    perror("send");
  }
}
} // namespace m_net

int main() {
  m_net::MNetServer server;

  // Start the server
  if (!server.setup("", 9034)) {
    std::cerr << "Failed to start server" << std::endl;
    return 1;
  }

  std::cout << "Server is running on port 9034..." << std::endl;

  // Listen for incoming connections
  server.listenForConnections();
}
