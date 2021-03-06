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
  discard_comments(true),
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

void lexer_t::set_discard_comments(bool discard_comments_) {
  discard_comments = discard_comments_;
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
        consume_escape();
      } else {
        go = false;
      }
    }
  }
  return std::string{anchor_name, static_cast<size_t>(cursor - anchor_name)};
}

std::shared_ptr<token_t> lexer_t::lex_url_token() {
  auto url_pos = pos;
  const char *anchor_url = cursor;
  std::string text;
  bool bad = false;
  bool go = true;
  enum {
    start,
    white_space_begin,
    white_space_end,
    single_quote,
    double_quote,
    url_body,
    bad_url,
  } state = start;
  do {
    char c = peek();
    switch (state) {
      case start: {
        switch (c) {
          case '\0': {
            text = std::string{anchor_url, static_cast<size_t>(cursor - anchor_url)};
            go = false;
            break;
          }
          case '\'': {
            pop();
            peek();
            url_pos = pos;
            state = single_quote;
            break;
          }
          case '"': {
            pop();
            peek();
            url_pos = pos;
            state = double_quote;
            break;
          }
          default: {
            if (isspace(c)) {
              state = white_space_begin;
              pop();
              break;
            }
            state = url_body;
            break;
          }
        }
        break;
      }

      case white_space_begin: {
        switch (c) {
          case '\0': {
            state = start;
            break;
          }
          case '\'': {
            pop();
            peek();
            state = single_quote;
            anchor_url = cursor;
            url_pos = pos;
            break;
          }
          case '"': {
            pop();
            peek();
            state = double_quote;
            anchor_url = cursor;
            url_pos = pos;
            break;
          }
          default: {
            if (isspace(c)) {
              pop();
              break;
            } else {
              state = url_body;
              url_pos = pos;
              anchor_url = cursor;
              break;
            }
          }
        }
        break;
      }

      case white_space_end: {
        if (isspace(c)) {
          pop();
          break;
        } else if (c == ')') {
          go = false;
          pop();
          break;
        }
        state = bad_url;
        pop();
        break;
      }

      case single_quote: {
        text = consume_string('\'');
        state = white_space_end;
        break;
      }

      case double_quote: {
        text = consume_string('"');
        state = white_space_end;
        break;
      }

      case url_body: {
        switch (c) {
          case '\0': {
            state = start;
            break;
          }
          case '\'':
          case '"':
          case '(': {
            bad = true;
            pop();
            break;
          }
          case ')': {
            text = std::string{anchor_url, static_cast<size_t>(cursor - anchor_url)};
            pop();
            go = false;
            break;
          }
          case '\\': {
            if (peek_is_escape()) {
              consume_escape();
              break;
            }
            bad = true;
            break;
          }
          default: {
            if (isspace(c)) {
              text = std::string{anchor_url, static_cast<size_t>(cursor - anchor_url)};
              state = white_space_end;
              pop();
              break;
            }
            pop();
            break;
          }
        }
        break;
      }

      /**
       * consume as much as we can until we get to stopping point
       * which is either right parent ( ) ) or EOF
       */
      case bad_url: {
        bad = true;
        switch (c) {
          case '\0': {
            go = false;
            break;
          }
          case '\'': {
            if (peek_is_escape()) {
              consume_escape();
              break;
            }
            break;
          }
          case ')': {
            go = false;
            break;
          }
        }
        break;
      }
    }
  } while (go);
  if (bad) {
    return token_t::make(url_pos, token_t::BAD_URL_TOKEN, std::move(text));
  }
  return token_t::make(url_pos, token_t::URL_TOKEN, std::move(text));
}

std::string lexer_t::consume_string(char ending_point) {
  const char *anchor_string = cursor;
  do {
    char c = peek();
    enum {
      start,
      back_slash,
    } state = start;
    switch (state) {
      case start: {
        switch (c) {
          case '\\': {
            state = back_slash;
            pop();
            break;
          }
          case '\n': {
            throw lexer_error_t(this, "unexpected new line in lex_string_token()::start");
          }
          case '\0': {
            return std::string{anchor_string, static_cast<size_t>(cursor - anchor_string)};
          }
          default: {
            if (c == ending_point) {
              std::string result{anchor_string, static_cast<size_t>(cursor - anchor_string)};
              pop();
              return result;
            }
            pop();
            break;
          }
        }
        break;
      }
      case back_slash: {
        switch (c) {
          case '\n': {
            pop();
            state = start;
            break;
          }
          default: {
            if (peek_is_escape()) {
              consume_escape();
            } else {
              pop();
            }
            state = start;
            break;
          }
        }
        break;
      }
    }
  } while (true);
}

std::shared_ptr<token_t> lexer_t::lex_ident_token() {
  set_anchor();
  auto text = consume_name();
  char c = peek();
  if (text == "url" && c == '(') {
    pop_anchor();
    pop();
    c = peek();
    return lex_url_token();
  } else if (c == '(') {
    pop_anchor();
    set_anchor();
    pop();
    peek();
    auto left_paren_token = token_t::make(anchor_pos, token_t::DELIM_TOKEN, pop_anchor());
    auto ident_token = token_t::make(anchor_pos, token_t::IDENT_TOKEN, std::move(text));
    return function_token_t::make(*ident_token, *left_paren_token);
  } else {
    pop_anchor();
    return token_t::make(anchor_pos, token_t::IDENT_TOKEN, std::move(text));
  }
}

std::shared_ptr<token_t> lexer_t::lex_numeric_token() {
  // dimension token
  auto flag = token_t::type_flag_t::INTEGER;
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
          flag = token_t::type_flag_t::NUMBER;
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
            flag = token_t::type_flag_t::NUMBER;
            state = point;
            pop();
            break;
          }
          case '%': {
            pop();
            state = percent;
            break;
          }
          default: {
            if (c == 'e' || c == 'E') {
              state = exponent;
              
              ++cursor;
              if (*cursor == '+' || *cursor == '-') {
                ++cursor;
                if (isdigit(*cursor)) {
                  state = exponent;
                  flag = token_t::type_flag_t::NUMBER;
                  --cursor;
                  --cursor;
                  pop();
                  break;
                }
                --cursor;
              } else if (isdigit(*cursor)) {
                state = exponent;
                flag = token_t::type_flag_t::NUMBER;
                --cursor;
                pop();
                break;
              }
              --cursor;
            }
            if (peek_is_identifier()) {
              auto text = pop_anchor();
              double num_value = stod(text);
              auto number = number_token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text), num_value);
              number->set_type_flag(flag);
              auto identifier = lex_ident_token();
              auto token = dimension_token_t::make(*number, *identifier);
              return std::move(token);
            } else {
              auto text = pop_anchor();
              double num_value = stod(text);
              auto token = number_token_t::make(anchor_pos, token_t::NUMBER_TOKEN, std::move(text), num_value);
              token->set_type_flag(flag);
              return std::move(token);
            }
          }
        }
        break;
      }

      case point: {
        if (consumed_point) {
          throw lexer_error_t(this, "unexpected extra point in lex_numeric_token()::point");
        }
        flag = token_t::type_flag_t::NUMBER;
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
        double num = stod(text);
        auto token = number_token_t::make(anchor_pos, token_t::PERCENTAGE_TOKEN, std::move(text), num);
        token->set_type_flag(flag);
        return std::move(token);
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
            return token_t::make(anchor_pos, token_t::COMMENT_TOKEN, std::move(text));
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
            return token_t::make(anchor_pos, token_t::COMMENT_TOKEN, std::move(text));
          }
          case '\0': {
            auto text = pop_anchor();
            return token_t::make(anchor_pos, token_t::COMMENT_TOKEN, std::move(text));
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

std::shared_ptr<token_t> lexer_t::lex_unicode_range() {
  const char *anchor_start = cursor;
  const char *anchor_end = nullptr;
  pos_t start_pos = pos;
  pos_t end_pos = pos;
  std::string hex_start_text;
  std::string hex_end_text;
  int num_consumed = 0;
  enum {
    start,
    hex_start,
    hex_end,
    question_range,
  } state = start;
  bool go = true;
  do {
    char c = peek();
    switch (state) {
      case start: {
        if (isxdigit(c)) {
          pop();
          c = peek();
          if (++num_consumed == 6) {
            hex_start_text = std::string{anchor_start, static_cast<size_t>(cursor - anchor_start)};
            state = hex_start;
          }
        } else if (c == '?') {
          pop();
          if (++num_consumed == 6) {
            state = question_range;
          }
        } else {
          throw lexer_error_t(this, "unexpected character in consume_unicode_range()::start");
        }
        break;
      }
      case hex_start: {
        if (c == '-') {
          const char *reset = cursor;
          pop();
          c = peek();
          if (isxdigit(c)) {
            end_pos = pos;
            anchor_end = cursor;
            state = hex_end;
            num_consumed = 0;
          } else {
            reset_cursor(reset);
            hex_end_text = hex_start_text;
            go = false;
          }
        } else {
          hex_end_text = hex_start_text;
          go = false;
        }
        break;
      }
      case hex_end: {
        if (isxdigit(c)) {
          pop();
          c = peek();
          if (++num_consumed == 6) {
            hex_end_text = std::string{anchor_end, static_cast<size_t>(cursor - anchor_end)};
            go = false;
          }
        } else {
          throw lexer_error_t(this, "unexpected character in consume_unicode_range()::hex_end");
        }
        break;
      }
      case question_range: {
        // interpret '?' as '0' for start
        // interpret '?' as 'f' for end
        for (int i = 0; i < 6; ++i) {
          char cc = *anchor_start;
          if (cc == '?') {
            hex_start_text += '0';
            hex_end_text += 'f';
          } else {
            hex_start_text += cc;
            hex_end_text += cc;
          }
          anchor_start++;
        }
        go = false;
        break;
      }
    }
  } while (go);
  int num_start = stoi(hex_start_text, nullptr, 16);
  int num_end = stoi(hex_end_text, nullptr, 16);
  auto start_hex_token = number_token_t::make(start_pos, token_t::NUMBER_TOKEN, std::move(hex_start_text), static_cast<double>(num_start));
  auto end_hex_token = number_token_t::make(end_pos, token_t::NUMBER_TOKEN, std::move(hex_end_text), static_cast<double>(num_end));
  start_hex_token->set_type_flag(token_t::type_flag_t::INTEGER);
  end_hex_token->set_type_flag(token_t::type_flag_t::INTEGER);
  return unicode_range_token_t::make(*start_hex_token, *end_hex_token);
}

std::vector<std::shared_ptr<token_t>> lexer_t::lex() {
  enum {
    start,
    whitespace,
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
            add_single_token(token_t::LEFT_BRACE_TOKEN);
            break;
          }
          case '}': {
            add_single_token(token_t::RIGHT_BRACE_TOKEN);
            break;
          }
          case '[': {
            add_single_token(token_t::LEFT_BRACKET_TOKEN);
            break;
          }
          case ']': {
            add_single_token(token_t::RIGHT_BRACKET_TOKEN);
            break;
          }
          case ':': {
            add_single_token(token_t::COLON_TOKEN);
            break;
          }
          case ';': {
            add_single_token(token_t::SEMICOLON_TOKEN);
            break;
          }
          case '"': {
            pop();
            peek();
            auto string_pos = pos;
            auto text = consume_string('"');
            tokens.push_back(token_t::make(string_pos, token_t::STRING_TOKEN, std::move(text)));
            break;
          }
          case '\'': {
            pop();
            peek();
            auto string_pos = pos;
            auto text = consume_string('\'');
            tokens.push_back(token_t::make(string_pos, token_t::STRING_TOKEN, std::move(text)));
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
            add_single_token(token_t::LEFT_PAREN_TOKEN);
            break;
          }
          case ')': {
            add_single_token(token_t::RIGHT_PAREN_TOKEN);
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
            add_single_token(token_t::COMMA_TOKEN);
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
          if (isxdigit(c) || c == '?') {
            pop_anchor();
            auto token = lex_unicode_range();
            tokens.push_back(token);
            state = start;
            break;
          }
        }

        reset_cursor(anchor);
        pop_anchor();
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
          if (!discard_comments) {
            tokens.push_back(token);
          }
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
        if (c == '-') {
          pop();
          c = peek();
          if (c == '>') {
            pop();
            auto text = pop_anchor();
            tokens.push_back(token_t::make(anchor_pos, token_t::CDC_TOKEN, std::move(text)));
            state = start;
            break;
          }
          reset_cursor(anchor);
          pop();
          c = peek();
          auto text = pop_anchor();
          auto token = token_t::make(anchor_pos, token_t::DELIM_TOKEN, std::move(text));
          tokens.push_back(token);
          break;
        } else if (isdigit(c) || c == '.') {
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
        tokens.push_back(token_t::make(anchor_pos, token_t::WHITESPACE_TOKEN));
        state = start;
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
