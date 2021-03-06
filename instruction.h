#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>

struct Instruction {
  Instruction(const std::vector<std::string>& tokens, const std::string& comment);
  std::vector<std::string> tokens;
  std::string comment;
};
#endif // INSTRUCTION_H
