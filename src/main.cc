#include <iostream>
#include <memory>
#include <ui/chat_ui.h>

int main() {
  std::cout << "Hello World" << std::endl;
  std::unique_ptr<m_net::ChatUI> ui = std::make_unique<m_net::ChatUI>();
  ui->start();
  ui->clean();
  return 0;
}
