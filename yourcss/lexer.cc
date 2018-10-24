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

char lexer_t::reset_cursor(const char *saved_cursor) {
  char c = peek();
  is_ready = false;
  next_cursor = saved_cursor;
  return c;
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

bool lexer_t::is_name_start(char c) {
  if (isalpha(c) || c == '_') {
    return true;
  }
  return false;
}

bool lexer_t::peek_is_identifier() {
  const char *cursor_anchor;
  char first = peek();
  if (first == '\0' || isspace(first)) {
    reset_cursor(cursor_anchor);
    return false;
  }

  // does first start with '-'
  if (first == '-') {
    char second = *(++cursor);
    if (second == '\0' || isspace(second)) {
      reset_cursor(cursor_anchor);
      return false;
    }
    // is second a valid escape
    if (second == '\\') {
      char third = *(++cursor);
      // is third a new line character
      if (third == '\n') {
        reset_cursor(cursor_anchor);
        return false;
      }
      // is valid start identifier
      reset_cursor(cursor_anchor);
      return true;
    }
    // is second a name start code point?
    if (is_name_start(second)) {
      reset_cursor(cursor_anchor);
      return true;
    }
    reset_cursor(cursor_anchor);
    return false;
  }

  // is first a name start
  if (is_name_start(first)) {
    return true;
  }

  // is first a valid escape
  if (first == '\\') {
    char second = *(++cursor);
    // is third a new line character
    if (second == '\n') {
      reset_cursor(cursor_anchor);
      return false;
    }
    // is valid escape
    reset_cursor(cursor_anchor);
    return true;
  }

  return false;
}

std::shared_ptr<token_t> lexer_t::lex_ident_token() {
  enum {
    start,
    start_dash,
    name,
    escape,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch(state) {
      case start: {
        switch (c) {
          case '-': {
            set_anchor();
            pop();
            state = start_dash;
            break;
          }
          default: {
            if (is_name_start(c)) {
              state = name;
              pop();
              break;
            }
            throw lexer_error_t(this, "unexpected char in lex_numeric_token()::start");
          }
        }
        break;
      }

      case start_dash: {
        if (is_name_start(c)) {
          pop();
          break;
        }
        pop();
        auto text = pop_anchor();
        return token_t::make(anchor_pos, token_t::PERCENT_TOKEN, std::move(text));
      }

      case name: {
        throw lexer_error_t(this, "todo");
      }

      case escape: {
        // TODO
        throw lexer_error_t(this, "unexpected error");
      }
    }
  } while (go);

  return nullptr;
}

std::shared_ptr<token_t> lexer_t::lex_numeric_token() {
  // dimension token
  std::shared_ptr<token_t> temp_number_token;
  std::shared_ptr<token_t> temp_identifier;

  bool consumed_point = false;
  bool consumed_e = false;

  enum {
    start,
    start_mod,
    number,
    exponent,
    point,
    percent,
    exponent_mod,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch(state) {
      case start: {
        switch (c) {
          case '+':
          case '-': {
            set_anchor();
            pop();
            state = start_mod;
            break;
          }
          case '.': {
            set_anchor();
            pop();
            state = point;
            break;
          }
          default: {
            if (isdigit(c)) {
              set_anchor();
              pop();
              state = number;
              break;
            }
            throw lexer_error_t(this, "unexpected char in lex_numeric_token()::start");
          }
        }
        break;
      }

      case start_mod: {
        if (c != '.' && !isdigit(c)) {
          throw lexer_error_t(this, "unexpected char in lex_numeric_token()::start");
        }
        state = number;
        break;
      }

      // number can only end with a digit
      case number: {
        if (isdigit(c)) {
          pop();
          break;
        }
        switch (c) {
          case '.': {
            state = point;
            pop();
            break;
          }
          case 'e':
          case 'E': {
            state = exponent;
            pop();
            break;
          }
          case '%': {
            pop();
            state = percent;
            break;
          }
          default: {
            if (peek_is_identifier()) {
              auto text = pop_anchor();
              temp_number_token = token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text));
              temp_identifier = lex_ident_token();
              throw lexer_error_t(this, "what now");
            } else {
              auto text = pop_anchor();
              return token_t::make(anchor_pos, token_t::PERCENT_TOKEN, std::move(text));
            }
          }
        }
        break;
      }

      case point: {
        if (consumed_point) {
          throw lexer_error_t(this, "unexpected extra point in lex_numeric_token()::point");
        }
        consumed_point = true;
        if (isdigit(c)) {
          state = number;
          pop();
          break;
        }
        throw lexer_error_t(this, "unexpected character after number decimal point in lex_number_token()::point");
      }

      case exponent: {
        if (consumed_e) {
          throw lexer_error_t(this, "unexpected extra exponent in lex_number_token()::exponent");
        }
        consumed_e = true;
        switch (c) {
          case '+':
          case '-': {
            pop();
            state = exponent_mod;
            break;
          }
          default: {
            if (isdigit(c)) {
              state = number;
              pop();
              break;
            }
            throw lexer_error_t(this, "unexpected character after number decimal point in lex_number_token()::exponent");
          }
        }
        break;
      }

      case exponent_mod: {
        if (!isdigit(c)) {
          throw lexer_error_t(this, "unexpected non digit in lex_number_token()::exponent_mod");
        }
        state = number;
        pop();
        break;
      }

      case percent: {
        pop();
        auto text = pop_anchor();
        auto token = token_t::make(anchor_pos, token_t::PERCENT_TOKEN, std::move(text));
        return token;
      }
    }
  } while (go);
  return nullptr;
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
    plus_start
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

      case plus_start: {
        if (isdigit(c)) {
          auto token = lex_numeric_token();
          tokens.push_back(token);
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
