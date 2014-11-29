#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>

struct Instruction {
  Instruction(const std::vector<std::string>& tokens);
  std::vector<std::string> tokens;
};
#endif // INSTRUCTION_H
