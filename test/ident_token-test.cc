#include <lick/lick.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

FIXTURE(identifier) {
  const char *src = R"(
    happyfaces
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::IDENT_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("happyfaces"));
}

FIXTURE(function_identifier) {
  const char *src = R"(
    happyfaces(123, 123, 123);
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::FUNCTION_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("happyfaces("));
  EXPECT_EQ(tokens[2]->get_text(), std::string("123"));
}

FIXTURE(url_identifier) {
  const char *src = R"(
    url(http://danielhood.com);
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_white_beg) {
  const char *src = R"(
    url( http://danielhood.com);
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_white_end) {
  const char *src = R"(
    url(http://danielhood.com );
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_single_quote) {
  const char *src = R"(
    url('http://danielhood.com');
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_single_quote_ws_begin) {
  const char *src = R"(
    url( 'http://danielhood.com');
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_single_quote_ws_end) {
  const char *src = R"(
    url('http://danielhood.com' );
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_single_quote_ws) {
  const char *src = R"(
    url(   'http://danielhood.com' );
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_double_quote_ws) {
  const char *src = R"(
    url( "http://danielhood.com" );
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}

FIXTURE(url_identifier_double_quote_ws_nice) {
  const char *src = R"(
    url("http://danielhood.com");
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("http://danielhood.com"));
}
