#include <yourcss/tokens/unicode_range_token.h>

namespace yourcss {

unicode_range_token_t::unicode_range_token_t(const token_t &start_, const token_t &end_):
  token_t(start_.get_pos(), token_t::UNICODE_RANGE_TOKEN, start_.get_text() + "-" + end_.get_text()),
  start(start_),
  end(end_) {}

token_t unicode_range_token_t::get_start() {
  return start;
}

token_t unicode_range_token_t::get_end() {
  return end;
}

std::shared_ptr<unicode_range_token_t> unicode_range_token_t::make(const token_t &start, const token_t &end) {
  return std::make_shared<unicode_range_token_t>(start, end);
}

}
