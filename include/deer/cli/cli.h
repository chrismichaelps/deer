#pragma once
#include <deer/types/archive.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace deer::cli {

/** @deer.cli.Command.Struct */
struct Command {
  /** @deer.cli.Command.Name */
  std::string name;
  /** @deer.cli.Command.Args */
  std::vector<std::string> args;
  /** @deer.cli.Command.Env */
  std::unordered_map<std::string, std::string> env;
  /** @deer.cli.Command.Cwd */
  std::optional<std::string> cwd;
  /** @deer.cli.Command.Shell */
  bool shell = false;
  /** @deer.cli.Command.Feed */
  std::string stdinFeed;
};

/** @deer.cli.Command.Make */
inline Command make(std::string name, std::vector<std::string> args = {}) {
  return Command{std::move(name), std::move(args), {}, std::nullopt, false, {}};
}

/** @deer.cli.Command.Env */
inline Command env(Command cmd, std::unordered_map<std::string, std::string> newEnv) {
  cmd.env.merge(newEnv);
  return cmd;
}

/** @deer.cli.Command.RunInShell */
inline Command runInShell(Command cmd, bool enabled = true) {
  cmd.shell = enabled;
  return cmd;
}

/** @deer.cli.Command.Feed */
inline Command feed(Command cmd, std::string input) {
  cmd.stdinFeed = std::move(input);
  return cmd;
}

/** @deer.cli.CommandExecutor.Interface */
class CommandExecutor {
public:
  virtual ~CommandExecutor() = default;
  /** @deer.cli.CommandExecutor.Execute */
  virtual int execute(const Command &cmd, std::string &stdout, std::string &stderr) = 0;
};

/** @deer.cli.DefaultExecutor */
class DefaultExecutor : public CommandExecutor {
public:
  /** @deer.cli.DefaultExecutor.Execute */
  int execute(const Command &cmd, std::string &stdout, std::string &stderr) override {
    std::string fullCmd = cmd.name;
    for (const auto &arg : cmd.args) {
      fullCmd += " " + arg;
    }
    if (cmd.shell) {
      fullCmd = "/bin/sh -c \"" + fullCmd + "\"";
    }
    FILE *pipe = popen(fullCmd.c_str(), "r");
    if (!pipe) {
      return -1;
    }
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) {
      stdout += buffer;
    }
    int exitCode = pclose(pipe);
    return exitCode;
  }
};

/** @deer.cli.Command.String */
inline int commandString(const Command &cmd, std::string &output) {
  DefaultExecutor executor;
  std::string stderr;
  return executor.execute(cmd, output, stderr);
}

/** @deer.cli.Command.ExitCode */
inline int commandExitCode(const Command &cmd) {
  std::string stdout;
  std::string stderr;
  DefaultExecutor executor;
  return executor.execute(cmd, stdout, stderr);
}

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