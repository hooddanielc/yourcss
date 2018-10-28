#include <yourcss/tokens/function_token.h>

namespace yourcss {

function_token_t::function_token_t(const token_t &identifier_, const token_t &left_paren_):
  token_t(identifier_.get_pos(), token_t::FUNCTION_TOKEN, identifier_.get_text() + left_paren_.get_text()),
  identifier(identifier_),
  left_paren(left_paren_) {}

token_t function_token_t::get_identifier() {
  return left_paren;
}

token_t function_token_t::get_left_paren() {
  return identifier;
}

std::shared_ptr<function_token_t> function_token_t::make(const token_t &at, const token_t &identifier) {
  return std::make_shared<function_token_t>(at, identifier);
}

function_token_t::~function_token_t() {};

}
