#include <iostream>
#include <yourcss/lexer.h>

const static char *glsl = R"(
  uniform float time;
  uniform vec2 mouse;
  uniform vec2 resolution;
)";

int main(int, char*[]) {

  yourcss::lexer_t lexer(glsl);
  auto tokens = lexer.lex();

  for (const auto &token: tokens) {
    std::cout << token << std::endl;
  }

  std::cout << "cool" << std::endl;
  return 0;
}
