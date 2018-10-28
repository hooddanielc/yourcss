#include <yourcss/tokens/at_keyword_token.h>

namespace yourcss {

at_keyword_token_t::at_keyword_token_t(const token_t &at_, const token_t &identifier_):
  token_t(at_.get_pos(), token_t::AT_KEYWORD_TOKEN, at_.get_text() + identifier_.get_text()),
  at(at_),
  identifier(identifier_) {}

token_t at_keyword_token_t::get_at() {
  return at;
}

token_t at_keyword_token_t::get_identifier() {
  return identifier;
}

std::shared_ptr<at_keyword_token_t> at_keyword_token_t::make(const token_t &at, const token_t &identifier) {
  return std::make_shared<at_keyword_token_t>(at, identifier);
}

at_keyword_token_t::~at_keyword_token_t() {};

}
