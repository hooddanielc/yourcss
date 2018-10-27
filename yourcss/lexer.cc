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

void lexer_t::print_tokens(const std::vector<std::shared_ptr<token_t>> &tokens) {
  for (const auto &token: tokens) {
    std::cout << (*token) << std::endl;
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
  is_ready = false;
  next_cursor = saved_cursor;
  char c = peek();
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

bool lexer_t::is_name_point(char c) {
  if (is_name_start(c) || isdigit(c) || c == '-') {
    return true;
  }
  return false;
}

bool lexer_t::is_name_start(char c) {
  if (isalpha(c) || c == '_' || is_non_ascii(c)) {
    return true;
  }
  return false;
}

bool lexer_t::is_non_ascii(char c) {
  if (static_cast<unsigned char> (c) > 127) {
    return true;
  }
  return false;
}

bool lexer_t::peek_is_identifier() {
  char first = peek();
  const char *cursor_anchor = cursor;
  if (first == '\0' || isspace(first)) {
    reset_cursor(cursor_anchor);
    return false;
  }

  // does first start with '-'
  if (first == '-') {
    pop();
    char second = peek();
    if (second == '\0' || isspace(second)) {
      reset_cursor(cursor_anchor);
      return false;
    }
    // is second a valid escape
    if (second == '\\') {
      pop();
      char third = peek();
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
    pop();
    char second = peek();
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

std::string lexer_t::consume_escape() {
  int num_hex_consumed = 0;
  const char *anchor_escape = cursor;
  enum {
    start,
    escape_body,
    hex_body,
  } state = start;
  bool go = true;
  while (go) {
    char c = peek();
    switch (state) {
      case start: {
        switch (c) {
          case '\\': {
            pop();
            state = escape_body;
          }
        }
        break;
      }

      case escape_body: {
        if (isxdigit(c)) {
          state = hex_body;
          num_hex_consumed += 1;
          pop();
        } else if (c != '\n') {
          pop();
          go = false;
        } else {
          throw lexer_error_t(this, "unexpected new line in escape body consume_escape()::escape_body");
        }
        break;
      }

      case hex_body: {
        if (isxdigit(c)) {
          if (num_hex_consumed >= 6) {
            throw lexer_error_t(this, "hex number can only contain 6 digits in consume_escape()::hex_body");
          }
          num_hex_consumed += 1;
          pop();
          break;
        }
        if (isspace(c)) {
          pop();
          go = false;
          break;
        }
        go = false;
        break;
      }

    }
  }
  return std::string{anchor_escape, static_cast<size_t>(cursor - anchor_escape)};
}

bool lexer_t::peek_is_escape() {
  const char *anchor_escape = cursor;
  char c = peek();
  if (c != '\\') {
    return false;
  }
  pop();
  c = peek();
  if (c == '\n') {
    reset_cursor(anchor_escape);
    return false;
  }
  reset_cursor(anchor_escape);
  return true;
}

std::string lexer_t::consume_name() {
  const char *anchor_name = cursor;
  bool go = true;
  while (go) {
    char c = peek();
    if (c == '\0') {
      go = false;
    } else {
      if (is_name_point(c)) {
        pop();
      } else if(peek_is_escape()) {
        pop();
      } else {
        go = false;
      }
    }
  }
  return std::string{anchor_name, static_cast<size_t>(cursor - anchor_name)};
}

std::shared_ptr<token_t> lexer_t::lex_ident_token() {
  set_anchor();
  enum {
    start,
    start_dash,
    name,
    start_escape,
    escape,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    // ghetto debug helper
    // if (!isspace(c)) {
    //   std::cout << " L: " << c << std::endl;
    // }
    switch(state) {
      case start: {
        switch (c) {
          case '-': {
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
          state = name;
          break;
        } else if (c == '\\') {
          state = start_escape;
          break;
        }
        throw lexer_error_t(this, "unexpected beginning of ident-token in lex_ident_token()::start_dash");
      }

      case name: {
        if (is_name_start(c) || c == '-' || isdigit(c)) {
          pop();
          break;
        } else if (c == '\\') {
          state = start_escape;
          break;
        }
        auto text = pop_anchor();
        return token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text));
      }

      case start_escape: {
        if (c == '\n') {
          pop();
          state = start;
          break;
        }
        state = escape;
        break;
      }

      case escape: {
        auto text = consume_escape();
        state = start;
        break;
      }
    }
  } while (go);

  return nullptr;
}

std::shared_ptr<token_t> lexer_t::lex_numeric_token() {
  // dimension token
  set_anchor();
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
            pop();
            state = start_mod;
            break;
          }
          case '.': {
            pop();
            state = point;
            break;
          }
          default: {
            if (isdigit(c)) {
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
        if (c == '.') {
          pop();
          state = point;
          break;
        }
        if (isdigit(c)) {
          state = number;
          break;
        }
        throw lexer_error_t(this, "unexpected char in lex_numeric_token()::start");
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
              auto number = token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text));
              auto identifier = lex_ident_token();
              auto token = dimension_token_t::make(*number, *identifier);
              return std::move(token);
            } else {
              auto text = pop_anchor();
              return token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text));
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

std::shared_ptr<token_t> lexer_t::lex_comment_token() {
  set_anchor();
  enum {
    start,
    slash_start,
    comment_body,
    asterisk_start,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch (state) {
      case start: {
        switch (c) {
          case '/': {
            state = slash_start;
            pop();
            break;
          }
          default: {
            throw ice_t(pos, __FILE__, __LINE__);
          }
        }
        break;
      }
      case slash_start: {
        switch (c) {
          case '*': {
            state = comment_body;
            pop();
            break;
          }
          default: {
            throw ice_t(pos, __FILE__, __LINE__);
          }
        }
        break;
      }
      case comment_body: {
        switch (c) {
          case '*': {
            state = asterisk_start;
            pop();
            break;
          }
          case '\0': {
            auto text = pop_anchor();
            return token_t::make(anchor_pos, token_t::COMMENT, std::move(text));
          }
          default: {
            pop();
            break;
          }
        }
        break;
      }
      case asterisk_start: {
        switch (c) {
          case '/': {
            pop();
            c = peek();
            auto text = pop_anchor();
            return token_t::make(anchor_pos, token_t::COMMENT, std::move(text));
          }
          case '\0': {
            auto text = pop_anchor();
            return token_t::make(anchor_pos, token_t::COMMENT, std::move(text));
          }
          default: {
            state = comment_body;
            pop();
            break;
          }
        }
        break;
      }
    }
  } while (go);
  return nullptr;
}

std::shared_ptr<token_t> lexer_t::lex_at_keyword_token() {
  set_anchor();
  char c = peek();
  if (c != '@') {
    // expects @keyword token to be known
    throw ice_t(pos, __FILE__, __LINE__);
  }
  pop();
  auto text = pop_anchor();
  auto at_token = token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text));
  auto ident_token = lex_ident_token();
  return at_keyword_token_t::make(*at_token, *ident_token);
}

std::string lexer_t::consume_unicode() {
  // TODO
  throw ice_t(pos, __FILE__, __LINE__);
}

std::vector<std::shared_ptr<token_t>> lexer_t::lex() {
  enum {
    start,
    whitespace,
    string_,
    string_single,
    hash_start,
    dollar_start,
    asterisk_start,
    plus_start,
    minus_start,
    period_start,
    slash_start,
    lesser_than_start,
    at_start,
    carrot_start,
    unicode_start,
    pipe_start,
    tilde_start,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch (state) {
      case start: {
        switch (c) {
          case 'u':
          case 'U': {
            set_anchor();
            state = unicode_start;
            pop();
            break;
          }
          case '\0': {
            set_anchor();
            go = false;
            break;
          }
          case '~': {
            set_anchor();
            state = tilde_start;
            pop();
            break;
          }
          case '\\': {
            if (peek_is_escape()) {
              auto token = lex_ident_token();
            } else {
              throw lexer_error_t(this, "unexpected escape character in lex()::start");
            }
            break;
          }
          case '|': {
            set_anchor();
            pop();
            state = pipe_start;
            break;
          }
          case '^': {
            state = carrot_start;
            set_anchor();
            pop();
            break;
          }
          case '{': {
            add_single_token(token_t::LEFT_BRACE);
            break;
          }
          case '}': {
            add_single_token(token_t::RIGHT_BRACE);
            break;
          }
          case '[': {
            add_single_token(token_t::LEFT_BRACKET);
            break;
          }
          case ']': {
            add_single_token(token_t::RIGHT_BRACKET);
            break;
          }
          case ':': {
            add_single_token(token_t::COLON);
            break;
          }
          case ';': {
            add_single_token(token_t::SEMICOLON);
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
          case '-': {
            state = minus_start;
            set_anchor();
            pop();
            break;
          }
          case ',': {
            add_single_token(token_t::COMMA);
            break;
          }
          case '.': {
            state = period_start;
            set_anchor();
            pop();
            break;
          }
          case '/': {
            state = slash_start;
            set_anchor();
            pop();
            break;
          }
          case '<': {
            state = lesser_than_start;
            set_anchor();
            pop();
            break;
          }
          case '@': {
            state = at_start;
            set_anchor();
            pop();
            break;
          }
          default: {
            if (isdigit(c)) {
              auto token = lex_numeric_token();
              tokens.push_back(token);
              break;
            } else if (isspace(c) || c == '\n') {
              set_anchor();
              pop();
              state = whitespace;
              break;
            } else if (is_name_start(c)) {
              auto token = lex_ident_token();
              tokens.push_back(token);
              state = start;
              break;
            } else {
              std::string delimeter_text(cursor, size_t(1));
              tokens.push_back(token_t::make(pos, token_t::DELIM_TOKEN, std::move(delimeter_text)));
              break;
            }
          }
        }
        break;
      }

      case tilde_start: {
        switch (c) {
          case '=': {
            pop();
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::INCLUDE_MATCH_TOKEN, std::move(text)));
            state = start;
            break;
          }
          default: {
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
            state = start;
            break;
          }
        }
        break;
      }

      case pipe_start: {
        switch (c) {
          case '=': {
            pop();
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::DASH_MATCH_TOKEN, std::move(text)));
            state = start;
            break;
          }
          case '|': {
            pop();
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::COLUMN_TOKEN, std::move(text)));
            state = start;
            break;
          }
          default: {
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
            state = start;
          }
        }
        break;
      }

      case unicode_start: {
        if (c == '+') {
          pop();
          c = peek();
          if (isdigit(c)) {
            consume_unicode();
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::UNICODE_RANGE_TOKEN, std::move(text)));
            state = start;
            break;
          }
        }
        reset_cursor(anchor);
        c = peek();
        auto token = lex_ident_token();
        tokens.push_back(token);
        state = start;
        break;
      }

      case carrot_start: {
        if (c == '=') {
          pop();
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::PREFIX_MATCH_TOKEN, std::move(text)));
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case lesser_than_start: {
        switch (c) {
          case '!': {
            pop();
            c = peek();
            if (c == '-') {
              pop();
              c = peek();
              if (c == '-') {
                pop();
                auto text = pop_anchor();
                tokens.push_back(token_t::make(anchor_pos, token_t::CDO_TOKEN, std::move(text)));
                state = start;
                break;
              }
            }
            reset_cursor(anchor);
            pop();
            auto text = pop_anchor();
            auto token = token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text));
            tokens.push_back(token);
            break;
          }
          default: {
            auto text = pop_anchor();
            auto token = token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text));
            tokens.push_back(token);
            state = start;
            break;
          }
        }
        break;
      }

      case at_start: {
        if (peek_is_identifier()) {
          reset_cursor(anchor);
          pop_anchor();
          auto token = lex_at_keyword_token();
          tokens.push_back(token);
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case slash_start: {
        if (c == '*') {
          reset_cursor(anchor);
          pop_anchor();
          auto token = lex_comment_token();
          tokens.push_back(token);
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case period_start: {
        if (isdigit(c)) {
          reset_cursor(anchor);
          pop_anchor();
          auto token = lex_numeric_token();
          tokens.push_back(token);
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case plus_start: {
        if (isdigit(c) || c == '.') {
          reset_cursor(anchor);
          pop_anchor();
          auto token = lex_numeric_token();
          tokens.push_back(token);
          state = start;
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
          state = start;
        }
        break;
      }

      case minus_start: {
        if (isdigit(c) || c == '.') {
          reset_cursor(anchor);
          pop_anchor();
          auto token = lex_numeric_token();
          tokens.push_back(token);
          state = start;
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
        if (peek_is_identifier()) {
          pop_anchor();
          auto text = consume_name();
          tokens.push_back(token_t::make(anchor_pos, token_t::HASH_TOKEN, std::move(text)));
        } else {
          auto text = pop_anchor();
          tokens.push_back(token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text)));
        }
        state = start;
        break;
      }

    }
  } while (go);
  return tokens;
}

}   // yourcss
