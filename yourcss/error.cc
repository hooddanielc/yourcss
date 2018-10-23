#include "error.h"

namespace yourcss {

const char *error_t::what() const noexcept {
  if (strm) {
    msg = strm->str();
    strm.reset();
  }
  return msg.c_str();
}

error_t::error_t(const pos_t &pos):
  strm(std::make_unique<std::ostringstream>()),
  sep_needed(false) {
  get_strm() << pos;
  end_section();
}

void error_t::end_section() {
  sep_needed = true;
}

std::ostream &error_t::get_strm() const {
  if (sep_needed) {
    (*strm) << "; ";
    sep_needed = false;
  }
  return *strm;
}

error_t::~error_t() = default;

}  // yourcss
