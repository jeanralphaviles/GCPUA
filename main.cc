#include "parser.h"

#include <fstream>
#include <string>

int main(int argc, char** argv) {
  if (argc != 4) {
    printf("You must specify an input file, an output file, and a rom size\n");
    exit(1);
  }
  std::ifstream input_file(argv[1]);
  std::ofstream output_file(argv[2]);
  if (!input_file.is_open()) {
    printf("Unable to open input file\n");
    exit(1);
  } else if (!output_file.is_open()) {
    printf("Unable to open output file\n");
    exit(1);
  }
  Parser parser;
  int lineNum = 1;
  std::string line;
  while(getline(input_file, line)) {
    parser.parseLine(line, lineNum++);
  }
  int romSize = std::stoi(argv[3]);
  output_file << parser.makeRom(argv[2], romSize).c_str();
  input_file.close();
  output_file.close();
  return 0;
}
