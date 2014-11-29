#include "token.h"

#include <string.h>

void token::tokenize(const std::string& line, std::vector<std::string>* tokens) {
  int lastToken = 0;
  for (int i = 0; i < line.length(); ++i) {
    if (isDelimeter(line[i]) || (i == line.length() - 1 && !isDelimeter(line[i++]))) {
      tokens->push_back(line.substr(lastToken, i - lastToken));
      while (i + 1 < line.length() && isDelimeter(line[i + 1])) {
        if (line[i] == ',') {
          tokens->push_back(",");
        }
        ++i;
      }
      lastToken = i + 1;
    }
  }
}

std::string token::unTokenize(const std::vector<std::string>& tokens) {
  std::string output;
  for (const auto& token : tokens) {
    output = output +  token + " ";
  }
  return output;
}

bool token::isDelimeter(const char& c) {
  switch(c) {
    case ' ':   return true;
    case '\n':  return true;
    case '\r':  return true;
    case '\t':  return true;
    case ',':   return true;
    default:    return false;
  }
}

bool token::isLabel(const std::string& token) {
  return !token.empty() && token.at(token.size() - 1) == ':';
}

bool token::isDirective(std::vector<std::string>& tokens) {
  return tokens.size() > 1 && tokens[1] == "EQU" || tokens[0] == "ORG";
}

bool token::isInstruction(const std::string& token) {
  if(token == "TAB")    return true;
  if(token == "TBA")    return true;
  if(token == "LDAA")   return true;
  if(token == "LDAB")   return true;
  if(token == "STAA")   return true;
  if(token == "STAB")   return true;
  if(token == "LDX")    return true;
  if(token == "LDY")    return true;
  if(token == "SUM_BA") return true;
  if(token == "SUM_AB") return true;
  if(token == "AND_BA") return true;
  if(token == "AND_AB") return true;
  if(token == "OR_BA")  return true;
  if(token == "OR_AB")  return true;
  if(token == "COMA")   return true;
  if(token == "COMB")   return true;
  if(token == "SHFA_L") return true;
  if(token == "SHFA_R") return true;
  if(token == "SHFB_L") return true;
  if(token == "SHFB_R") return true;
  if(token == "INX")    return true;
  if(token == "INY")    return true;
  if(token == "BEQ")    return true;
  if(token == "BNE")    return true;
  if(token == "BN")     return true;
  if(token == "BP")     return true;
  else                  return false;
}

std::string token::opcode(const std::string& token) {
  if(token == "TAB")    return "00";
  if(token == "TBA")    return "01";
  if(token == "SUM_BA") return "14";
  if(token == "SUM_AB") return "15";
  if(token == "AND_BA") return "16";
  if(token == "AND_AB") return "17";
  if(token == "OR_BA")  return "18";
  if(token == "OR_AB")  return "19";
  if(token == "COMA")   return "1A";
  if(token == "COMB")   return "1B";
  if(token == "SHFA_L") return "1C";
  if(token == "SHFA_R") return "1D";
  if(token == "SHFB_L") return "1E";
  if(token == "SHFB_R") return "1F";
  if(token == "INX")    return "30";
  if(token == "INY")    return "31";
  if(token == "BEQ")    return "20";
  if(token == "BNE")    return "21";
  if(token == "BN")     return "22";
  if(token == "BP")     return "23";
}

std::vector<std::string> token::getOpCodes(const Instruction& instruction, std::unordered_map<std::string, int>& labels_) {
  std::vector<std::string> output;
  std::string token = instruction.tokens[0];
  if (numArgs(token) == 0) {
    output.push_back(opcode(token));
  } else {
    if(isImmediate(instruction)) {
      std::string immAddr = instruction.tokens[1];
      immAddr.erase(0, 1);
      if (immAddr.front() == '$') {
        immAddr.erase(0, 1);
      }
      if (labels_.find(immAddr) != labels_.end()) {
        immAddr = std::to_string(labels_[immAddr]);
        while (immAddr.size() < numAddrBits(instruction)) {
          immAddr = "0" + immAddr;
        }
      }
      if (immAddr.size() != numAddrBits(instruction)) {
        printf("Immediate address %s is not %i bits long\n", immAddr.c_str(), numAddrBits(instruction));
        exit(1);
      }
      output.push_back(immOpCodes(instruction.tokens[0]));
      if (immAddr.size() == 4) {
        output.push_back(immAddr.substr(2, 4));
      }
      output.push_back(immAddr.substr(0, 2));
    } else if (isExtended(instruction)) {
      std::string exAddr = instruction.tokens[1];
      if (exAddr.front() == '$') {
        exAddr.erase(0, 1);
      }
      if (labels_.find(exAddr) != labels_.end()) {
        exAddr = std::to_string(labels_[exAddr]);
        while (exAddr.size() < numAddrBits(instruction)) {
          exAddr = "0" + exAddr;
        }
      }
      if (exAddr.size() != numAddrBits(instruction)) {
        printf("Extended address %s is not %i bits long\n", exAddr.c_str(), numAddrBits(instruction));
        exit(1);
      }
      output.push_back(exOpCodes(instruction.tokens[0]));
      output.push_back(exAddr.substr(2, 4));
      output.push_back(exAddr.substr(0, 2));
    } else if (isIndexed(instruction)) {
      std::string indexedAddr = instruction.tokens[1];
      while (indexedAddr.size() < numAddrBits(instruction)) {
        indexedAddr = "0" + indexedAddr;
      }
      if (indexedAddr.size() != numAddrBits(instruction)) {
        printf("Indexed address %s is not %i bits long\n", indexedAddr.c_str(), numAddrBits(instruction));
        exit(1);
      }
      output.push_back(indexedOpCodes(instruction));
      std::string displacement = instruction.tokens[1];
      if (displacement.size() == 1) {
       displacement = "0" + displacement;
      }
      output.push_back(displacement);
    } else if (isAbsolute(instruction)) {
      std::string absAddr = instruction.tokens[1];
      if (labels_.find(absAddr) != labels_.end()) {
        char* hexNum = new char[256];
        sprintf(hexNum, "%X", labels_[absAddr]);
        absAddr = std::string(hexNum);
        while (absAddr.size() < numAddrBits(instruction)) {
          absAddr = "0" + absAddr;
        }
        if (absAddr.size() > 2) {
          absAddr = absAddr.substr(absAddr.size() - 2, absAddr.size());
        }
        delete hexNum;
      } else {
        printf("Cannot find label %s\n", absAddr.c_str());
        exit(1);
      }
      output.push_back(opcode(instruction.tokens[0]));
      output.push_back(absAddr);
    } else {
      printf("Cannot determine instruction\n");
      exit(1);
    }
  }
  return output;
}

std::string token::immOpCodes(const std::string& instruction) {
  if (instruction == "LDAA")  return "02";
  if (instruction == "LDAB")  return "03";
  if (instruction == "LDX")   return "08";
  if (instruction == "LDY")   return "09";
  else                        return "";
}

std::string token::exOpCodes(const std::string& instruction) {
  if (instruction == "LDAA")  return "04";
  if (instruction == "LDAB")  return "05";
  if (instruction == "STAA")  return "06";
  if (instruction == "STAB")  return "07";
  if (instruction == "LDX")   return "0A";
  if (instruction == "LDY")   return "0B";
  else                        return "";
}

std::string token::indexedOpCodes(const Instruction& instruction) {
  if (instruction.tokens[0] == "LDAA") {
    if (instruction.tokens[3] == "X") {
      return "0C";
    } else if (instruction.tokens[3] == "Y") {
      return "0D";
    }
  } else if (instruction.tokens[0] == "LDAB") {
    if (instruction.tokens[3] == "X") {
      return "0E";
    } else if (instruction.tokens[3] == "Y") {
      return "0F";
    }
  } else if (instruction.tokens[0] == "STAA") {
    if (instruction.tokens[3] == "X") {
      return "10";
    } else if (instruction.tokens[3] == "Y") {
      return "11";
    }
  } else if (instruction.tokens[0] == "STAB") {
    if (instruction.tokens[3] == "X") {
      return "12";
    } else if (instruction.tokens[3] == "Y") {
      return "13";
    }
  }
  printf("Couldn\'t get opcode for indexed address %s\n", instruction.tokens[0].c_str());
  exit(1);
}

bool token::isImmediate(const Instruction& instruction) {
  if (instruction.tokens.size() != 2) {
    return false;
  }
  if (numArgs(instruction.tokens[0]) != 1) {
    return false;
  }
  if (instruction.tokens[1].front() != '#') {
    return false;
  }
  return true;
}

bool token::isExtended(const Instruction& instruction) {
  if (instruction.tokens.size() != 2) {
    return false;
  }
  if (numArgs(instruction.tokens[0]) != 1) {
    return false;
  }
  if (instruction.tokens[1].front() == '#') {
    return false;
  }
  std::string ir = instruction.tokens[0];
  if (ir == "BEQ")   return false;
  if (ir == "BNE")   return false;
  if (ir == "BN")    return false;
  if (ir == "BP")    return false;
  else               return true;
}

bool token::isIndexed(const Instruction& instruction) {
  if (instruction.tokens.size() != 4) {
    return false;
  }
  if (numArgs(instruction.tokens[0]) != 1) {
    return false;
  }
  if (instruction.tokens[2].front() != ',') {
    return false;
  }
  return true;
}

bool token::isAbsolute(const Instruction& instruction) {
  if (instruction.tokens.size() != 2) {
    return false;
  }
  if (numArgs(instruction.tokens[0]) != 1) {
    return false;
  }
  std::string ir = instruction.tokens[0];
  if (ir == "BEQ")  return true;
  if (ir == "BNE")  return true;
  if (ir == "BN")   return true;
  if (ir == "BP")   return true;
  else              return false;
}

int token::numAddrBits(const Instruction& instruction) {
  if (isExtended(instruction)) {
    return 4;
  } else if (isImmediate(instruction) &&
      (instruction.tokens[0] == "LDX" || instruction.tokens[0] == "LDY")) {
    return 4;
  } else {
    return 2;
  }
}

int token::numArgs(const std::string& instruction) {
  if(instruction == "TAB")    return 0;
  if(instruction == "TBA")    return 0;
  if(instruction == "LDAA")   return 1;
  if(instruction == "LDAB")   return 1;
  if(instruction == "STAA")   return 1;
  if(instruction == "STAB")   return 1;
  if(instruction == "LDX")    return 1;
  if(instruction == "LDY")    return 1;
  if(instruction == "SUM_BA") return 0;
  if(instruction == "SUM_AB") return 0;
  if(instruction == "AND_BA") return 0;
  if(instruction == "AND_AB") return 0;
  if(instruction == "OR_BA")  return 0;
  if(instruction == "OR_AB")  return 0;
  if(instruction == "COMA")   return 0;
  if(instruction == "COMB")   return 0;
  if(instruction == "SHFA_L") return 0;
  if(instruction == "SHFA_R") return 0;
  if(instruction == "SHFB_L") return 0;
  if(instruction == "SHFB_R") return 0;
  if(instruction == "INX")    return 0;
  if(instruction == "INY")    return 0;
  if(instruction == "BEQ")    return 1;
  if(instruction == "BNE")    return 1;
  if(instruction == "BN")     return 1;
  if(instruction == "BP")     return 1;
  return -1;
}

int token::numOpCodes(const Instruction& instruction) {
  if (numArgs(instruction.tokens[0]) == 0) {
    return 1;
  }
  if (isIndexed(instruction) || isAbsolute(instruction)) {
    return 2;
  }
  if (isExtended(instruction)) {
    return 3;
  }
  if (isImmediate(instruction)) {
    if (instruction.tokens[0] == "LDX" || instruction.tokens[0] == "LDY") {
      return 3;
    } else {
      return 2;
    }
  }
  return 0;
}

