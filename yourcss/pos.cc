#include "pos.h"

namespace yourcss {

pos_t::pos_t() noexcept: line_number(1), col_number(1) {}

void pos_t::next_col() {
  ++col_number;
}

void pos_t::next_line() {
  ++line_number;
  col_number = 1;
}

int pos_t::get_line() const {
  return line_number;
}

int pos_t::get_col() const {
  return col_number;
}

std::ostream &operator<<(std::ostream &strm, const pos_t &that) {
  return strm
    << "line " << that.line_number
    << ", col " << that.col_number;
}

}
