#include "lexer.h"

namespace yourcss {

lexer_t::lexer_error_t::lexer_error_t(const lexer_t *lexer, const char *msg):
  error_t(lexer->pos) { get_strm() << msg; }

lexer_t::lexer_error_t::~lexer_error_t() = default;

void lexer_t::print_tokens(const std::vector<token_t> &tokens) {
  for (const auto &token: tokens) {
    std::cout << token << std::endl;
  }
}

lexer_t::lexer_t(const char *next_cursor_):
  next_cursor(next_cursor_),
  is_ready(false),
  anchor(nullptr) {}

char lexer_t::peek() const {
  if (!is_ready) {
    cursor = next_cursor;
    pos = next_pos;
    switch (*cursor) {
      case '\0': {
        break;
      }
      case '\n': {
        ++next_cursor;
        next_pos.next_line();
        break;
      }
      default: {
        ++next_cursor;
        next_pos.next_col();
      }
    }  // switch
    is_ready = true;
  }
  return *cursor;
}

char lexer_t::pop() {
  char c = peek();
  is_ready = false;
  return c;
}

void lexer_t::set_anchor() const {
  anchor_pos = pos;

  if (anchor) {
    throw ice_t(pos, __FILE__, __LINE__);
  }

  anchor = cursor;
}

std::string lexer_t::pop_anchor() {
  if (!anchor) {
    for (const auto &token: tokens) {
      std::cout << token << std::endl;
    }
    throw ice_t(pos, __FILE__, __LINE__);
  }

  std::string text(anchor, static_cast<size_t>(cursor - anchor));
  anchor = nullptr;
  return text;
}

void lexer_t::add_single_token(token_t::kind_t kind) {
  set_anchor();
  pop();
  pop_anchor();
  tokens.push_back(token_t::make(anchor_pos, kind));
}

std::vector<std::shared_ptr<token_t>> lexer_t::lex() {
  enum {
    start,
    whitespace,
    string_,
    string_single,
    hash_start,
    hash_escape,
    hash_id,
    hash_id_start,
    dollar_start,
    asterisk_start,
    plus_start,
    numeric_start,
    number_start,
    number_before_decimal_point,
    number_after_decimal_point,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch (state) {

      case start: {
        switch (c) {
          case '\0': {
            set_anchor();
            go = false;
            break;
          }
          case '"': {
            state = string_;
            set_anchor();
            pop();
            break;
          }
          case '\'': {
            state = string_single;
            set_anchor();
            pop();
            break;
          }
          case '#': {
            state = hash_start;
            set_anchor();
            pop();
            break;
          }
          case '$': {
            state = dollar_start;
            set_anchor();
            pop();
            break;
          }
          case '(': {
            add_single_token(token_t::LEFT_PAREN);
            break;
          }
          case ')': {
            add_single_token(token_t::RIGHT_PAREN);
            break;
          }
          case '*': {
            state = asterisk_start;
            set_anchor();
            pop();
            break;
          }
          case '+': {
            state = plus_start;
            set_anchor();
            pop();
            break;
          }
          default: {
            if (isspace(c) || c == '\n') {
              set_anchor();
              pop();
              state = whitespace;
            } else {
              std::string msg("unexpected character ");
              msg += c;
              throw lexer_error_t(this, msg.c_str());
            }
          }
        }
        break;
      }

      case numeric_start: {
        if (c == '+' || c == '-') {
          pop();
          c = peek();
        }
        bool is_before_decimal = true;
        if (c == '.') {
          pop();
          c = peek();
          is_before_decimal = false;
        }
        bool consumed_e = false;
        bool invalid_number = false;
        while (c != '\0') {
          if (isspace(c) || c == '\n') {
            break;
          }
          if (!isdigit(c)) {
            switch (c) {
              case 'e':
              case 'E': {
                if (!consumed_e) {
                  pop();
                  c = peek();
                  if (c == '+' || c == '-') {
                    pop();
                    c = peek();
                  }
                  consumed_e = true;
                } else {
                  invalid_number = true;
                }
                break;
              }
              case '.': {
                if (!is_before_decimal) {
                  invalid_number = true;
                }
                break
              }
            }
          }
        }
        switch (c) {
          case '+':
          case '-': {
            pop();
            c = peek();
            if (isdigit(c)) {
              state = number_before_decimal_point;
            } else if (c == '.') {
              state = number_after_decimal_point;
            } else {
              auto text = pop_anchor();
              tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
              state = start;
            }
            break
          }
          default: {
            break;
          }
        }
      }

      case plus_start: {
        if (isdigit(c)) {
          state = number_start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case asterisk_start: {
        if (c == '=') {
          pop();
          c = peek();
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::SUBSTRING_MATCH_TOKEN, std::move(text)));
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case dollar_start: {
        if (c == '=') {
          pop();
          c = peek();
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::SUFFIX_MATCH_TOKEN, std::move(text)));
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case whitespace: {
        if (isspace(c) || c == '\n') {
          pop();
          break;
        }
        pop_anchor();
        tokens.push_back(token_t::make(anchor_pos, token_t::WHITESPACE));
        state = start;
        break;
      }

      case string_: {
        if (c == '"') {
          state = start;
          pop();
          c = peek();
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::STRING_TOKEN, std::move(text)));
        } else if (c == '\'') {
          throw lexer_error_t(this, "unexpected new line in string string_");
        } else {
          pop();
        }
        break;
      }

      case string_single: {
        if (c == '\'') {
          state = start;
          pop();
          c = peek();
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::STRING_TOKEN, std::move(text)));
        } else if (c == '\'') {
          throw lexer_error_t(this, "unexpected new line in string string_single");
        } else {
          pop();
        }
        break;
      }

      case hash_start: {
        switch (c) {
          case '\\': {
            state = hash_escape;
            pop();
            break;
          }
          default: {
            if (isspace(c) || c == '\n') {
              state = start;
              auto text = pop_anchor();
              tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
            } else {
              state = hash_id_start;
              pop();
            }
            break;
          }
        }
        break;
      }

      case hash_escape: {
        if (c != '\n' && !isspace(c)) {
          state = hash_id_start;
          pop();
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case hash_id_start: {
        if (isalpha(c) || c == '_') {
          state = hash_id;
          pop();
        } else {
          throw lexer_error_t(this, "expected start of identifier");
        }
        break;
      }

      case hash_id: {
        if (isalnum(c) || c == '_' || c == '-') {
          pop();
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::HASH_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

    }
  } while (go);
  return tokens;
}

}   // yourcss
