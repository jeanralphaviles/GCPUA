#ifndef PARSER_H
#define PARSER_H

#include "instruction.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Parser {
 public:
  Parser();
  void parseLine(const std::string& raw_line, const int lineNum);
  std::string makeRom(const char* fileName, const int romSize);
 protected:
  bool isBlankLine(const std::string& line);
  void trimString(std::string* line);
  int numInstructions();
  const std::pair<const std::string, int>* labelAt(int romLocation);
 private:
  std::unordered_map<std::string, int> labels_;
  std::vector<std::pair<std::vector<Instruction>, int> > rom_;
  int currLocation_;
};

#endif // PARSER_H
