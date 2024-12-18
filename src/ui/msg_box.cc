#include <chrono>
#include <thread>
#include <ui/msg_box.h>
#include <ui/ui_utils.h>

namespace m_net {
MsgBox::MsgBox(int nlines, int ncols, int begin_y, int begin_x) {
  this->nlines = nlines;
  msg_window = newwin(nlines, ncols, begin_y, begin_x);
  newMsg = true; // trigger intial message box drawing
}

MsgBox::~MsgBox() {
  stopped = true;
  safeClear([this]() { wclear(msg_window); });
  safeClear([this]() { wrefresh(msg_window); });
  delwin(msg_window);
}

void MsgBox::start() {
  std::thread msg_check_thread(&MsgBox::checkForMessages, this);
  while (1) {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return newMsg || stopped; });
    if (stopped) {
      break;
    }
    draw();
  }
  msg_check_thread.join();
}

void MsgBox::stop() {
  stopped = true;
  cv.notify_all();
}

void MsgBox::addMsg(const char *msg) {
  std::unique_lock<std::mutex> lock(mutex);
  std::string msg_string = msg;
  messages.push_back(msg_string);
  newMsg = true;
  cv.notify_one();
}

void MsgBox::draw() {
  // std::unique_lock<std::mutex> lock(mutex);
  // cv.wait(lock, [this] { return newMsg || stopped; });
  // if (stopped) {
  //  return;
  //}
  safeClear([this]() { wclear(msg_window); });
  int start = messages.size() - (nlines - 2);
  start = start < 0 ? 0 : start;
  int len = messages.size();

  // for most recent messages
  for (int i = start; i < len; i++) {
    mvwprintw(msg_window, i - start + 1, 1, "%s", messages[i].c_str());
  }
  box(msg_window, 0, 0);
  safeClear([this]() { wrefresh(msg_window); });
  newMsg = false;
}

void MsgBox::checkForMessages() { mockMsgCheck(); }

void MsgBox::mockMsgCheck() {
  std::vector<std::string> mock_msgs = {"1", "2", "3", "4", "5", "6"};
  for (const std::string &msg : mock_msgs) {
    if (stopped) {
      break;
    }
    addMsg(msg.c_str());
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
}

} // namespace m_net
