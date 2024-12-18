#include <ui/msg_box.h>
#include <ui/ui_utils.h>

#include <iostream>

namespace m_net {
MsgBox::MsgBox(int nlines, int ncols, int begin_y, int begin_x) {
  this->nlines = nlines;
  msg_window = newwin(nlines, ncols, begin_y, begin_x);
}

MsgBox::~MsgBox() {
  safeClear([this]() { wclear(msg_window); });
  safeClear([this]() { wrefresh(msg_window); });
  delwin(msg_window);
}

void MsgBox::addMsg(const char *msg) {
  std::unique_lock<std::mutex> lock(mutex);
  std::string msg_string = msg;
  messages.push_back(msg_string);
  newMsg = true;
  cv.notify_one();
}

void MsgBox::draw() {
  // safeClear([this]() { wclear(msg_window); });
  // box(msg_window, 0, 0);
  // safeClear([this]() { wrefresh(msg_window); });
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock, [this] { return newMsg || stopped; });
  if (stopped) {
    return;
  }
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

} // namespace m_net
