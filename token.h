#ifndef TOKEN_H
#define TOKEN_H

#include "instruction.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace token {
  void tokenize(std::string line, std::vector<std::string>* const token, std::string* const comment);
  std::string unTokenize(const std::vector<std::string>& tokens);
  bool isDelimeter(const char& c);
  bool isLabel(const std::string& token);
  bool isDirective(std::vector<std::string>& tokens);
  bool isInstruction(const std::string& token);
  std::string opcode(const std::string& token);
  std::vector<std::string> getOpCodes(const Instruction& instruction, std::unordered_map<std::string, int>& labels_);
  std::string immOpCodes(const std::string& instruction);
  std::string exOpCodes(const std::string& instruction);
  std::string indexedOpCodes(const Instruction& instruction);
  bool isImmediate(const Instruction& instruction);
  bool isExtended(const Instruction& instruction);
  bool isIndexed(const Instruction& instruction);
  bool isAbsolute(const Instruction& instruction);
  int numAddrBits(const Instruction& instruction);
  int numArgs(const std::string& instruction);
  int numOpCodes(const Instruction& instruction);
} // namespace

#endif // TOKEN_H
