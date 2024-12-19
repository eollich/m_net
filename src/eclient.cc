#include <iostream>
#include <net/tcp_echo_client.h>
#include <string>

namespace m_net {
int EchoClient::run() {
  TCPClient client;

  if (!client.setup("127.0.0.1", 9034)) {
    std::cerr << "Failed to connect to server" << std::endl;
    return 1;
  }

  std::cout << "Connected to the server!" << std::endl;

  std::string message;
  while (true) {
    std::cout << "Enter message ('quit' to exit): ";
    std::getline(std::cin, message);

    if (message == ";quit") {
      std::cout << "Exiting." << std::endl;
      break;
    }

    if (!client.sendData(message)) {
      std::cerr << "failed to send data" << std::endl;
      continue;
    }

    std::cout << "msg sent: " << message << std::endl;

    std::string response = client.receiveData();
    if (!response.empty()) {
      std::cout << "Received from server: " << response << std::endl;
    } else {
      std::cerr << "Failed to receive response from server" << std::endl;
    }
  }

  return 0;
}
}; // namespace m_net

int main() {
  m_net::EchoClient ec;
  ec.run();
}
