#include <cstring>
#include <deer/cli/cli.h>
#include <deer/ops/archive.h>
#include <deer/ops/compress.h>
#include <deer/types/archive.h>
#include <deer/types/config.h>
#include <deer/types/errors.h>
#include <deer/types/version.h>
#include <iostream>

/** @deer.main */
int main(int argc, char **argv) {
  std::cout << deer::FULL_NAME << " v" << deer::VERSION << "\n\n";
  auto state = deer::ops::load(deer::config::ARCHIVE_PATH);
  if (state.recentTurns.empty() && state.memorySlots.empty()) {
    std::cout << "[Deer] New archive initialized.\n";
  }

  std::string cmd = (argc > 1) ? argv[1] : deer::config::CMD_HELP;

  if (cmd == deer::config::CMD_RESUME) {
    deer::cli::resume(state);
  } else if (cmd == deer::config::CMD_ADD) {
    deer::cli::add(state);
    deer::ops::save(state, deer::config::ARCHIVE_PATH);
  } else if (cmd == deer::config::CMD_COMPRESS) {
    deer::config::CompressionLevel level = deer::config::DEFAULT_COMPRESSION_LEVEL;
    if (argc > 2) {
      if (std::strcmp(argv[2], deer::config::CMD_ARG_FAST) == 0) {
        level = deer::config::CompressionLevel::Fast;
      } else if (std::strcmp(argv[2], deer::config::CMD_ARG_MAX) == 0) {
        level = deer::config::CompressionLevel::Max;
      } else if (std::strcmp(argv[2], deer::config::CMD_ARG_BALANCED) == 0) {
        level = deer::config::CompressionLevel::Balanced;
      }
    }
    deer::ops::compressArchiveWithLevel(state, level);
    deer::ops::save(state, deer::config::ARCHIVE_PATH);
  } else if (cmd == deer::config::CMD_DECOMPRESS) {
    deer::ops::decompressArchive(state);
  } else {
    deer::cli::help();
  }

  return 0;
}