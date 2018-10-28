#include <yourcss/tokens/number_token.h>

namespace yourcss {

number_token_t::number_token_t(const pos_t &pos, kind_t kind, double value_):
  token_t(pos, kind),
  value(value_) {}

number_token_t::number_token_t(const pos_t &pos, kind_t kind, std::string &&text, double value_):
  token_t(pos, kind, std::move(text)),
  value(value_) {}

double number_token_t::get_value() {
  return value;
}

std::shared_ptr<number_token_t>
number_token_t::make(const pos_t &pos, kind_t kind, double value) {
  return std::make_shared<number_token_t>(pos, kind, value);
}

std::shared_ptr<number_token_t>
number_token_t::make(const pos_t &pos, kind_t kind, std::string &&text, double value) {
  return std::make_shared<number_token_t>(pos, kind, std::move(text), value);
}

number_token_t::~number_token_t() {}

}
