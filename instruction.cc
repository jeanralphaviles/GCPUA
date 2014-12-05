#include "instruction.h"

  Instruction::Instruction(const std::vector<std::string>& tokens, const std::string& comment) {
  this->tokens = tokens;
  this->comment = comment;
}
