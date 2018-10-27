#include <lick/lick.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

FIXTURE(comment_normal) {
  const char *src = R"(
    /* This is a nice comment */
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::COMMENT, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("/* This is a nice comment */"));
}

FIXTURE(comment_nice) {
  const char *src = R"(
    /**
     * Comments can be nice
     */
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::COMMENT, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string(R"(/**
     * Comments can be nice
     */)"
  ));
}

FIXTURE(comment_nasty) {
  const char *src = R"(
    /* *O*R*  *N*A*S*T*Y* */
  )";
  auto tokens = lexer_t(src).lex();
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
  EXPECT_EQ(token_t::kind_t::COMMENT, tokens[1]->get_kind());
  EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
  EXPECT_EQ(tokens[1]->get_text(), std::string("/* *O*R*  *N*A*S*T*Y* */"));
}
