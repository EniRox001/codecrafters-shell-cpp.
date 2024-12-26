#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

std::string get_path(std::string command) {
  std::string abs_path = std::getenv("PATH");
  std::stringstream ss(abs_path);
  std::string path;
  while (!ss.eof()) {
    getline(ss, path, ':');
    std::string abs_path = path + "/" + command;
    if (std::filesystem::exists(abs_path)) {
      return abs_path;
    }
  }
  return "";
}

int main() {

  // Shell built-in commands
  std::vector<std::string> built_ins = {"echo", "exit", "type"};

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
      for (int i = 0; i < built_ins.size(); i++) {
        if (val == built_ins[i]) {
          std::cout << val << " is a shell builtin" << std::endl;
          found = true;
          break;
        }
      }
      if (!found) {
        std::string path = get_path(val);

        if (path.empty()) {
          std::cout << val << ": not found" << std::endl;
        } else {
          std::cout << val << " is " << path << std::endl;
        }
      }
      continue;
    }

    std::cout << input << ": command not found" << std::endl;
  }
}
