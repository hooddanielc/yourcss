#include <lick/lick.h>
#include <yourcss/lexer.h>
#include <yourcss/token.h>

using namespace yourcss;

FIXTURE(at_regular_name) {
  try {
    const char *src = R"(
      @media
    )";
    auto tokens = lexer_t(src).lex();
    EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
    EXPECT_EQ(token_t::kind_t::AT_KEYWORD_TOKEN, tokens[1]->get_kind());
    EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
    EXPECT_EQ(tokens[1]->get_text(), std::string("@media"));
  } catch (const std::exception &e) {
    std::cout << "WRONG: " << e.what() << std::endl;
  }
}

FIXTURE(at_dash_name) {
  try {
    const char *src = R"(
      @-moz-media
    )";
    auto tokens = lexer_t(src).lex();
    EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[0]->get_kind());
    EXPECT_EQ(token_t::kind_t::AT_KEYWORD_TOKEN, tokens[1]->get_kind());
    EXPECT_EQ(token_t::kind_t::WHITESPACE, tokens[2]->get_kind());
    EXPECT_EQ(tokens[1]->get_text(), std::string("@-moz-media"));
  } catch (const std::exception &e) {
    std::cout << "WRONG: " << e.what() << std::endl;
  }
}
