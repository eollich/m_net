#include <ui/text_input.h>
#include <ui/ui_utils.h>

#include <ncurses.h>

namespace m_net {
TextInput::TextInput(
    const char *placeholder, int nlines, int ncols, int begin_y, int begin_x,
    std::function<bool(const char *, const char *)> validator) {
  input_win = newwin(nlines, ncols + 2, begin_y, begin_x);
  this->placeholder = placeholder;
  this->validator = validator;
  buffer = (char *)calloc(ncols, sizeof(char));
  max_buffer_sz = ncols;
}

TextInput::~TextInput() {
  free(buffer);
  safeClear([this]() { wclear(input_win); });
  safeClear([this]() { wrefresh(input_win); });
  delwin(input_win);
}

void TextInput::draw() {
  safeClear([this]() { wclear(input_win); });
  box(input_win, 0, 0);
  if (buffer[0] == '\0') {
    mvwprintw(input_win, 1, 1, "--%s--", placeholder);
  } else {
    mvwprintw(input_win, 1, 1, "%s", buffer);
  }
  safeClear([this]() { wrefresh(input_win); });
}

std::tuple<bool, const char *> TextInput::getInput() {
  int ch;
  int idx = 0;
  while (idx < max_buffer_sz - 1) {
    if (idx == 0) {
      draw();
    }
    ch = wgetch(input_win);

    if (ch == 10) {
      break;
    } else if (ch == KEY_BACKSPACE || ch == 127) {
      if (idx > 0) {
        idx--;
        buffer[idx] = '\0';
      }
    } else if (isprint(ch)) {
      buffer[idx++] = ch;
      buffer[idx] = '\0';
    }
    draw();
  }
  // bool status = validate();
  return std::make_pair(false, buffer);
}

} // namespace m_net
