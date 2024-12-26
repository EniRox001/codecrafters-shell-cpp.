#include <iostream>
#include <ostream>
#include <string>
#include <vector>

int main() {

  // Shell built-in commands
  std::vector<std::string> builtIns = {"echo", "exit", "type"};

  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Uncomment this block to pass the first stage
  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "exit 0") {
      break;
    }
    // check for echo command
    if (input.find("echo ") == 0) {
      std::cout << input.substr(5) << std::endl;
      continue;
    }

    // check for built-in command
    if (input.find("type ") == 0) {
      std::string val = input.substr(5);

      bool found = false;
      for (int i = 0; i < builtIns.size(); i++) {
        if (val == builtIns[i]) {
          std::cout << val << " is a shell builtin" << std::endl;
          found = true;
          break;
        }
      }
      if (!found) {
        std::cout << val << ": not found" << std::endl;
      }
      continue;
    }

    std::cout << input << ": command not found" << std::endl;
  }
}
