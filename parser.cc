#include "instruction.h"
#include "parser.h"
#include "token.h"

#include <algorithm>
#include <stdlib.h>
#include <string.h>

Parser::Parser() {
  comments_ = std::unordered_map<int, std::string>();
  labels_ = std::unordered_map<std::string, int>();
  rom_ = std::vector<std::pair<std::vector<Instruction>, int> >();
  currLocation_ = 0;
  rom_.push_back(std::make_pair(std::vector<Instruction>(), currLocation_));
}

void Parser::parseLine(const std::string& raw_line, const int lineNum) {
  std::string line = raw_line;
  if (isBlankLine(line)) {
    return;
  } else if (isCommentLine(line)) {
    line.erase(0, line.find("*") + 1);
    if (isBlankLine(line)) {
      // Ignore blank comment lines
      return;
    }
    trimString(&line);
    line = "-- " + line + "\n";
    if (comments_.find(currLocation_) != comments_.end()) {
      comments_[currLocation_].append(line);
    } else {
      comments_[currLocation_] = line;
    }
    return;
  }
  trimString(&line);
  std::string comment;
  std::vector<std::string> tokens;
  token::tokenize(line, &tokens, &comment);

  if (token::isLabel(tokens.front())) {
    std::string label = tokens.front();
    if (label.back() == ':') {
      label.erase(label.size() - 1, 1);
    }
    labels_[label] = currLocation_;
    tokens.erase(tokens.begin());
    if (tokens.empty()) {
      return;
    }
  }

  if (token::isDirective(tokens)) {
    if (tokens[0] == "ORG") {
      int base = 10;
      if (tokens[1].at(0) == '$') {
        tokens[1] = tokens[1].substr(1, tokens[1].size());
        base = 16;
      }
      try {
        currLocation_ = stoi(tokens[1], nullptr, base);
      } catch (int e) {
        printf("%s\n", raw_line.c_str());
        printf("Cannot calculate address at line %i\n", lineNum);
      }
      rom_.push_back(std::make_pair(std::vector<Instruction>(), currLocation_));
    } else if (tokens[1] == "EQU") {
      std::string labelName = tokens[0];
      int base = 10;
      int location;
      if (tokens[2].at(0) == '$') {
        tokens[2] = tokens[2].substr(1, tokens[2].size());
        base = 16;
      }
      try {
        location = stoi(tokens[2], nullptr, base);
      } catch (int e) {
        printf("%s\n", raw_line.c_str());
        printf("Cannot calculate address at line %i\n", lineNum);
      }
      labels_[labelName] = location;
    } else if (tokens[1] == "DS.B") {
      std::string labelName = tokens[0];
      int base = 10;
      int size;
      if (tokens[2].at(0) == '$') {
        tokens[2] = tokens[2].substr(1, tokens[2].size());
        base = 16;
      }
      try {
        size = stoi(tokens[2], nullptr, base);
      } catch (int e) {
        printf("%s\n", raw_line.c_str());
        printf("Cannot calculate address at line %i\n", lineNum);
      }
      labels_[labelName] = currLocation_;
      Instruction instruction(tokens, std::string());
      rom_[rom_.size() - 1].first.push_back(instruction);
      currLocation_ += size;
    } else if (tokens[1] == "DC.B") {
      std::string labelName = tokens[0];
      labels_[labelName] = currLocation_;
      Instruction instruction(tokens, std::string());
      rom_[rom_.size() - 1].first.push_back(instruction);
      currLocation_++;
    }
    return;
  }

  if (!token::isInstruction(tokens[0])) {
    printf("%s\n", raw_line.c_str());
    printf("No instruction found on line number %i\n", lineNum);
    exit(1);
  }

  Instruction instruction(tokens, comment);
  rom_[rom_.size() - 1].first.push_back(instruction);
  currLocation_ += token::numOpCodes(instruction);
}

std::string Parser::makeRom(const char* fileName, const int romSize) {
  char** rom = new char*[romSize];
  std::vector<std::pair<Instruction, int>> instructionLabels;
  for (int i = 0; i < romSize; ++i) {
    rom[i] = new char[3];
    memset(rom[i], '\0', 3);
  }
  for (const auto& memoryLocation : rom_) {
    int romLocation = memoryLocation.second;
    for (const auto& instruction : memoryLocation.first) {
      if (romLocation >= romSize) {
        printf("Rom out of bounds\n");
        exit(1);
      }
      std::vector<std::string> opCodes = token::getOpCodes(instruction, labels_);
      instructionLabels.push_back(std::make_pair(instruction, romLocation));
      for (const auto& opCode : opCodes) {
        if (rom[romLocation][0] != '\0') {
          printf("Overwrote memory address %X\n", romLocation);
          exit(1);
        }
        strcpy(rom[romLocation++], opCode.c_str());
      }
    }
  }

  std::sort(instructionLabels.begin(), instructionLabels.end(),
      [](const std::pair<Instruction, int>& lhs, const std::pair<Instruction, int>& rhs) {return lhs.second < rhs.second;});

  std::string output = "% File name = " + std::string(fileName) + "\t\t%\n";
  output += "%**************************************************************************%\n";
  output += "%Created with Gator-CPU assembler written by Jean-Ralph Aviles - 11/28/2014%\n";
  output += "%**************************************************************************%\n\n";
  output += "DEPTH = ";
  output += std::to_string(romSize);
  output += ";\t% Address Bus Size %\n";
  output += "WIDTH = 8;\t% Data Format %\n\n";
  output += "ADDRESS_RADIX = HEX;\t% Address Format %\n";
  output += "DATA_RADIX = HEX;\t% Data Format %\n\n";
  output += "CONTENT\nBEGIN\n\n";

  int blankCounter = 0;
  int instructionLabelsCounter = 0;
  int i;
  char* temp = new char[256];

  for (i = 0; i < romSize; ++i) {
    if (rom[i][0] != '\0') {
      if (blankCounter > 5) {
        sprintf(temp, "[%X..%X]\t\t: 00;\t %% zero memory %% \n\n", i - blankCounter, i - 1);
        output += std::string(temp);
        blankCounter = 0;
      } else if (blankCounter > 0) {
        for (int j = i - blankCounter; j < i; ++j) {
          sprintf(temp, "%X\t\t: 00;\n", j);
          output += std::string(temp);
        }
        output += "\n";
        blankCounter = 0;
      }
      if (comments_.find(i) != comments_.end()) {
        sprintf(temp, "%s", comments_[i].c_str());
        output += std::string(temp);
      }
      sprintf(temp, "%X\t\t: %s;", i, rom[i]);
      output += std::string(temp);
      if (i == instructionLabels[instructionLabelsCounter].second) {
        auto* labelHere = labelAt(i);
        if (labelHere != nullptr) {
          sprintf(temp, " %% %s: ", labelHere->first.c_str());
          output += std::string(temp);
        } else {
          sprintf(temp, " %% ");
          output += std::string(temp);
        }
        sprintf(temp, "%s %%", (token::unTokenize(instructionLabels[instructionLabelsCounter].first.tokens) + instructionLabels[instructionLabelsCounter].first.comment).c_str());
        output += std::string(temp);
      }
      sprintf(temp, "\n");
      output += std::string(temp);
      if (i == instructionLabels[instructionLabelsCounter].second + token::numOpCodes(instructionLabels[instructionLabelsCounter].first) - 1) {
        sprintf(temp, "\n");
        output += std::string(temp);
        ++instructionLabelsCounter;
      }
    } else {
      ++blankCounter;
    }
  }
  if (blankCounter > 2) {
    sprintf(temp, "[%X..%X]\t\t: 00;\t %% zero rest of memory %% \n", i - blankCounter, i - 1);
    output += std::string(temp);
  }
  output += "END;\n";

  for (int i = 0; i < romSize; ++i) {
    delete rom[i];
  }
  delete temp;
  delete rom;
  return output;
}

bool Parser::isBlankLine(const std::string& line) {
  if (line.empty()) {
    return true;
  }
  for (const char& c : line) {
    if (c > 32 && c < 127) {
      return false;
    }
  }
  return true;
}

bool Parser::isCommentLine(const std::string& line) {
  for (const char& c : line) {
    if (c > 32) {
      return c == '*';
    }
  }
  return false;
}

void Parser::trimString(std::string* line) {
  for (int i = 0; i < line->length(); ++i) {
    if (line->at(i) > 32 && line->at(i) < 127) {
      line->erase(0, i);
      break;
    }
  }
  for (int i = line->length() - 1; i > -1; --i) {
    if (line->at(i) > 32 && line->at(i) < 127) {
      line->erase(i+1, line->length());
      break;
    }
  }
}

int Parser::numInstructions() {
  int output = 0;
  for (const auto& memoryLocation : rom_) {
    output += memoryLocation.first.size();
  }
  return output;
}

const std::pair<const std::string, int>* Parser::labelAt(int romLocation) {
  for (const auto& label : labels_) {
    if (label.second == romLocation) {
      return &label;
    }
  }
  return nullptr;
}

