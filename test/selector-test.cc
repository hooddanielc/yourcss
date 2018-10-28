#include <lick/lick.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

FIXTURE(at_regular_name) {
  const char *src = R"(
    .cool {
      something: 123;
      asdf: url(http://danielhood.com);
    }
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::DELIM_TOKEN, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::IDENT_TOKEN, tokens[2]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[3]->get_kind());
  EXPECT_EQ(token_t::kind_t::LEFT_BRACE_TOKEN, tokens[4]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[5]->get_kind());
  EXPECT_EQ(token_t::kind_t::IDENT_TOKEN, tokens[6]->get_kind());
  EXPECT_EQ(token_t::kind_t::COLON_TOKEN, tokens[7]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[8]->get_kind());
  EXPECT_EQ(token_t::kind_t::NUMBER_TOKEN, tokens[9]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON_TOKEN, tokens[10]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[11]->get_kind());
  EXPECT_EQ(token_t::kind_t::IDENT_TOKEN, tokens[12]->get_kind());
  EXPECT_EQ(token_t::kind_t::COLON_TOKEN, tokens[13]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[14]->get_kind());
  EXPECT_EQ(token_t::kind_t::URL_TOKEN, tokens[15]->get_kind());
  EXPECT_EQ(token_t::kind_t::SEMICOLON_TOKEN, tokens[16]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE_TOKEN, tokens[17]->get_kind());
  EXPECT_EQ(token_t::kind_t::RIGHT_BRACE_TOKEN, tokens[18]->get_kind());
}
