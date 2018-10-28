#pragma once

#include <cctype>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include "error.h"
#include "ice.h"
#include "token.h"
#include "pos.h"
#include "tokens/dimension_token.h"
#include "tokens/at_keyword_token.h"
#include "tokens/function_token.h"
#include "tokens/unicode_range_token.h"
#include "tokens/number_token.h"
namespace yourcss {

/* Convert source text into a vector of tokens. */
class lexer_t final {

public:

  /* An error in lexing. */
  class lexer_error_t final: public error_t {

  public:

    /* Report the position and what we found there. */
    lexer_error_t(const lexer_t *lexer, const char *msg);

    virtual ~lexer_error_t();

  };  // lexer_t::lexer_error_t

  /* Heper method to print tokens returned from lex */
  static void print_tokens(const std::vector<token_t> &tokens);

  /* Heper method to print tokens returned from lex */
  static void print_tokens(const std::vector<std::shared_ptr<token_t>> &tokens);

  /* Used by our public lex function. */
  lexer_t(const char *next_cursor);

  /* Used by our public lex function. */
  std::vector<std::shared_ptr<token_t>> lex();

  /* Lex a numeric token. */
  std::shared_ptr<token_t> lex_numeric_token();

  /* Lex an ident token. */
  std::shared_ptr<token_t> lex_ident_token();

  /* Lex comment token. */
  std::shared_ptr<token_t> lex_comment_token();

  /* Lex at-keyword-token */
  std::shared_ptr<token_t> lex_at_keyword_token();

  /* Lex a url token. Assumes `url(` has already been consumed */
  std::shared_ptr<token_t> lex_url_token();

  /* Lex a unicode token. Assumes u+ has already been consumed */
  std::shared_ptr<token_t> lex_unicode_range();

  /* Consume string token */
  std::string consume_string(char ending_point);

  /* Consume a name token */
  std::string consume_name();

  /* Consume escaped code point */
  std::string consume_escape();

  /* Checks if next 1-3 input code points would start an identifier */
  bool peek_is_identifier();

  /* Checks if two code points are valid escape. */
  bool peek_is_escape();

  /* Checks if the next item in stream is a name code point */
  bool is_name_point(char c);

  /* Check if sequence of current cursor would start a name */
  bool is_name_start(char c);

  /* Set the string and position to be tokenized, and keep state */
  char reset_cursor(const char *next_);

  /* Check if char is non-ascii */
  bool is_non_ascii(char c);

  /* Is valid escape */
  bool is_valid_escape();

  /* If true comments are not returned during tokenization */
  void set_discard_comments(bool);

private:

  /* Return the current character from the source text but don't advance to
     the next one. */
  char peek() const;

  /* Return the current character from the source text and advance to the
     next one. */
  char pop();

  /* Sets an anchor at the current cursor position. Throws if
     anchor is alread defined. Anchor should not be set if
     a previous anchor was dropped. */
  void set_anchor() const;

  /* Return the lexeme starting from anchor, and set anchor to null */
  std::string pop_anchor();

  /* Add a token at the current position, set anchor, advance 1 character
     and reset anchor. Used for tokens using only one character that can
     not be included in multi character tokens. ex. left_paren, right_paren
     etc.*/
  void add_single_token(token_t::kind_t kind);

  /* Temporarily holds tokens while lexing */
  std::vector<std::shared_ptr<token_t>> tokens;

  /* Should comments be ignored? Defaults to true */
  bool discard_comments;

  /* Our next position within the source text. */
  mutable const char *next_cursor;

  /* The (line, col) of next_cursor. */
  mutable pos_t next_pos;

  /* If true, then cursor and pos, below, are valid; otherwise,
     those fields contain junk.  Peeking makes us ready, popping makes us
     unready. */
  mutable bool is_ready;

  /* Our current position within the source text, when ready. */
  mutable const char *cursor;

  /* The (line, col) cursor, when ready. */
  mutable pos_t pos;

  /* The (line, col) cursor of an anchor. Usually the start of a lexeme. */
  mutable pos_t anchor_pos;

  /* Position in source text for anchor */
  mutable const char *anchor;

};  // lexer_t

}   // yourcss
