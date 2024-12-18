#include <ui/chat_ui.h>

#include <iostream>
#include <ncurses.h>

#include <thread>
#include <ui/msg_box.h>
#include <ui/text_input.h>

namespace m_net {
ChatUI::ChatUI() {
  setlocale(LC_ALL, "");
  initscr();
  refresh();
  noecho();
  curs_set(FALSE);
  getmaxyx(stdscr, w_height, w_width);
  refresh();
}

ChatUI::~ChatUI() { endwin(); }

void ChatUI::start() {
  std::unique_ptr<TextInput> login = std::make_unique<TextInput>(
      "Server:port", 3, 50, w_height - w_height / 2 + 10,
      w_width / 2 - (50 / 2),
      [](const char *username, const char *password) { return true; });

  std::unique_ptr<MsgBox> msg_box =
      std::make_unique<MsgBox>(w_height / 2, w_width - 2, 1, 1);

  // msg_box->addMsg("Hello world");
  // msg_box->addMsg("Hello world2");
  // msg_box->addMsg("Hello world3");
  // msg_box->addMsg("Hello world4");
  // msg_box->addMsg("Hello world5");
  // msg_box->addMsg("Hello world6");
  // msg_box->addMsg("Hello world7");
  // msg_box->addMsg("Hello world8");
  // msg_box->addMsg("Hello world9");
  // msg_box->addMsg("Hello world10");
  // msg_box->addMsg("Hello world11");
  // msg_box->addMsg("Hello world12");
  // msg_box->addMsg("Hello world13");
  // msg_box->addMsg("Hello world14");
  // msg_box->addMsg("Hello world15");
  // msg_box->addMsg("Hello world16");
  // msg_box->addMsg("Hello world17");
  // msg_box->addMsg("Hello world18");
  // msg_box->draw();

  std::thread mbt(&MsgBox::start, msg_box.get());

  login->draw();
  login->getInput();
  msg_box->stop();
  mbt.join();
  clear();
  refresh();
}
void ChatUI::clean() {}

} // namespace m_net
