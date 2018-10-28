#include <yourcss/tokens/dimension_token.h>

namespace yourcss {

dimension_token_t::dimension_token_t(const token_t &number_, const token_t &identifier_):
  token_t(number_.get_pos(), token_t::DIMENSION_TOKEN, number_.get_text() + identifier_.get_text()),
  number(number_),
  identifier(identifier_) {}

token_t dimension_token_t::get_number() {
  return number;
}

token_t dimension_token_t::get_identifier() {
  return identifier;
}

std::shared_ptr<dimension_token_t> dimension_token_t::make(const token_t &number, const token_t &identifier) {
  return std::make_shared<dimension_token_t>(number, identifier);
}

dimension_token_t::~dimension_token_t() {};

}
