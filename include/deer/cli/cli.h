#pragma once
#include <deer/types/archive.h>
#include <iostream>
#include <string>
#include <vector>

namespace deer::cli {

/** @deer.cli.resume */
inline void resume(const ArchiveState &state) {
  json payload = {{"system", "You are resuming from the Deer memory archive."},
                  {"archive",
                   {{"coded", state.codedArchive},
                    {"slots", state.memorySlots},
                    {"recent", state.recentTurns}}}};
  std::cout << payload.dump(2) << "\n";
}

/** @deer.cli.add */
inline void add(ArchiveState &state) {
  std::string input((std::istreambuf_iterator<char>(std::cin)), {});
  if (!input.empty() && input.back() == '\n') {
    input.pop_back();
  }
  if (!input.empty()) {
    state.recentTurns.push_back({{"role", "user"}, {"content", input}});
    std::cout << "[Deer] Turn added.\n";
  }
}

/** @deer.cli.help */
inline void help() {
  std::cout << "Usage: deer [resume | add | compress]\n";
}

} // namespace deer::cli