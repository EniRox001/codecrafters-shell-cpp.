#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

// Function to locate the program in the PATH environment variable
std::string get_path(const std::string &command) {
  char *path_env = std::getenv("PATH");
  if (!path_env)
    return "";
  std::stringstream ss(path_env);
  std::string path;
  while (std::getline(ss, path, ':')) {
    std::string abs_path = path + "/" + command;
    if (std::filesystem::exists(abs_path)) {
      return abs_path;
    }
  }
  return "";
}

std::vector<std::string> split_input(const std::string &input) {
  std::vector<std::string> tokens;
  std::string current_token;
  bool is_quoted = false;
  char quote_char = '\0';

  for (size_t i = 0; i < input.size(); ++i) {
    char c = input[i];

    if (c == '\'' || c == '\"') {
      if (is_quoted && c == quote_char) {
        is_quoted = false;
      } else if (!is_quoted) {
        is_quoted = true;
        quote_char = c;
      } else {
        current_token += c;
      }
    } else if (std::isspace(c) && !is_quoted) {
      if (!current_token.empty()) {
        tokens.push_back(current_token);
        current_token.clear();
      }
    } else {
      current_token += c;
    }
  }

  if (!current_token.empty()) {
    tokens.push_back(current_token);
  }

  return tokens;
}
int main() {
  // Shell built-in commands
  std::vector<std::string> built_ins = {"echo", "exit", "type", "pwd"};

  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Main loop
  while (true) {
    std::cout << "$ ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
      continue; // Ignore empty input
    }

    // Handle "exit 0" command
    if (input == "exit 0") {
      break;
    }

    // Split input into tokens
    std::vector<std::string> tokens = split_input(input);
    if (tokens.empty()) {
      continue;
    }

    std::string command = tokens[0];
    std::vector<std::string> arguments(tokens.begin() + 1, tokens.end());

    // Check for built-in commands
    if (command == "echo") {
      for (size_t i = 0; i < arguments.size(); ++i) {
        std::cout << arguments[i];
        if (i < arguments.size() - 1) {
          std::cout << " ";
        }
      }
      std::cout << std::endl;
      continue;
    }

    if (command == "type") {
      bool found = false;
      for (const auto &cmd : built_ins) {
        if (!arguments.empty() && arguments[0] == cmd) {
          std::cout << arguments[0] << " is a shell builtin" << std::endl;
          found = true;
          break;
        }
      }
      if (!found) {
        std::string path = get_path(arguments.empty() ? "" : arguments[0]);
        if (path.empty()) {
          std::cout << arguments[0] << ": not found" << std::endl;
        } else {
          std::cout << arguments[0] << " is " << path << std::endl;
        }
      }
      continue;
    }

    if (command == "pwd") {
      std::cout << std::filesystem::current_path().string() << std::endl;
      continue;
    }

    if (command == "cd") {
      if (arguments.empty() || arguments[0] == "~") {

        std::filesystem::current_path(std::getenv("HOME"));
        continue;
      } else if (std::filesystem::exists(arguments[0])) {
        std::filesystem::current_path(arguments[0]);
        continue;
      } else {
        std::cerr << "cd: " << arguments[0] << ": No such file or directory"
                  << std::endl;
        continue;
      }
    }

    if (command == "cat") {
      if (arguments.empty()) {
        std::cerr << "cat: missing file operand" << std::endl;
        continue;
      }

      for (const auto &file : arguments) {
        // open the file
        std::ifstream ifs(file);

        if (!ifs) {
          std::cerr << "cat: " << file << ": No such file or directory"
                    << std::endl;
          continue;
        }

        std::string line;
        while (std::getline(ifs, line)) {
          std::cout << line << "";
        }
      }
      std::cout << std::endl;
      continue;
    }

    // Locate the command in the PATH
    std::string path = get_path(command);
    if (path.empty()) {
      std::cout << command << ": command not found" << std::endl;
      continue;
    }

    // Execute the command with arguments
    std::string exec_command = path;
    for (const auto &arg : arguments) {
      exec_command += " " + arg;
    }

    int result = std::system(exec_command.c_str());
    if (result != 0) {
      std::cerr << "Error executing command" << std::endl;
    }
  }
}
